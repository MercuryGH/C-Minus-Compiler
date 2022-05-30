#include <iostream>
#include <stdexcept>
#include <boost/format.hpp>
#include "CodeGenerator.h"
#include "Compiler.h"

inline void printSymbolTable(const SymbolTable &symbolTable) {
    for (const auto &[k, v] : symbolTable) {
        // std::cout << k << ": ";
        for (const auto &[k1, v1] : v) {
            string printStr = (boost::format("  %s: {%d, %s, %d}\n") 
                % k1 % v1.variableIndex 
                % v1.variableType % v1.varSize).str();
            // std::cout << printStr;
        }
        // std::cout << "\n";
    }
}

// a += b
inline void appendInstructions(vector<Instruction> &a, const vector<Instruction> &b) {
    a.insert(a.end(), b.begin(), b.end());
}

void CodeGenerator::throwIdNotFoundErr(const string &id) 
{
    throw std::runtime_error((
        boost::format("id %s not found in symbol table!") % id)
    .str());
}

/**
 * @brief entry of CodeGenerator
 * @return vector<Instruction> all instructions generated
 */
vector<Instruction> CodeGenerator::generate() const
{
    const InstructionMap instMap = createInstMap();
    // std::cout << "After entry\n";

    unordered_map<string, int> funcCallMap;
    
    vector<Instruction> insts = mergeInstMap(instMap, funcCallMap);

    translateCall(insts, funcCallMap);

    return insts;
}

/**
 * @brief Generate load-literal (constant) instruction
 * @return vector<Instruction> contains only a LDC instruction
 */
vector<Instruction> CodeGenerator::generateLiteral(const AST *root, const string &scopeName) const
{
    return { Instruction("ldc", root->getTokenStr()) };
}

/**
 * @brief Get all instructions in a compound statement
 * @return Array of instructions
 * @details LocalVariableDecl is not generated. Pushing them 
 * into the stack is done during a function call.
 * @EBNF CompoundStmt -> '{' LocalVariableDecl StmtList '}'
 */
vector<Instruction> CodeGenerator::generateCompoundStmt(const AST *root, const string &scopeName) const
{
    return generateStmtList(root->getChildren().at(1), scopeName);
}

/**
 * @brief Get all instructions in a statement list
 * @return Array of instructions
 * @EBNF StmtList -> { Stmt }
 */
vector<Instruction> CodeGenerator::generateStmtList(const AST *root, const string &scopeName) const
{
    vector<Instruction> insts;

    for (const AST* const stmt : root->getChildren()) {
        // std::cout << "Before generate Stmt\n";
        const vector<Instruction> stmtInsts = generateStmt(stmt, scopeName);
        appendInstructions(insts, stmtInsts);
    }

    return insts;
}

/**
 * @brief Reduce Stmt
 * @EBNF Stmt -> ExprStmt | CompoundStmt | IfStmt | WhileStmt | ReturnStmt
 */
vector<Instruction> CodeGenerator::generateStmt(const AST *root, const string &scopeName) const
{
    if (root == nullptr) {
        return {};
    }

    switch (root->getTokenType()) {
        case TokenType::EXPR:
            return generateExpr(root, scopeName);
        case TokenType::COMPOUND_STMT:
            return generateCompoundStmt(root, scopeName);
        case TokenType::IF_STMT:
            return generateIfStmt(root, scopeName);
        case TokenType::WHILE_STMT:
            return generateWhileStmt(root, scopeName);
        case TokenType::RETURN_STMT:
            return generateReturnStmt(root, scopeName);
        default:
            throw std::runtime_error("Invalid token while generating Stmt");
    }
}

/**
 * @EBNF IfStmt -> if '(' Expr ')' Stmt [ else Stmt ]
 * 
 * @details
 * IF Statement
 * START:
 *     cmp
 *     jz END
 *     ... # ifInstSize
 * END: 
 * 
 * IF-ELSE Statement
 * START:
 *     cmp
 *     jz ELSE
 *     ... # ifInstSize
 *     JMP END
 * ELSE:
 *     ... # elseInstSize
 * END:
 */
vector<Instruction> CodeGenerator::generateIfStmt(const AST *root, const string &scopeName) const
{
    const auto &children = root->getChildren();
    auto insts = generateExpr(children.at(0), scopeName);
    auto ifInsts = generateStmt(children.at(1), scopeName);

    if (children.size() == 2) {
        // if statement

        // +1 considers the JZ inst added.
        const int jzOffset = ifInsts.size() + 1;
        insts.emplace_back("jz", std::to_string(jzOffset));
        appendInstructions(insts, ifInsts);
    } else {
        // if-else statement
        // std::cout << "?: ";
        // std::cout << children.size() << " " << children.at(2)->getTokenStr() << "\n";
        auto elseInsts = generateStmt(children.at(2), scopeName);
        // std::cout << "!\n";

        const int jmpOffset = elseInsts.size() + 1;
        ifInsts.emplace_back("jmp", std::to_string(jmpOffset));

        const int jzOffset = ifInsts.size() + 1;
        insts.emplace_back("jz", std::to_string(jzOffset));

        appendInstructions(insts, ifInsts);
        appendInstructions(insts, elseInsts);
    }

    return insts;
}

/**
 * @EBNF WhileStmt -> while '(' Expr ')' Stmt
 * 
 * @details
 * START:
 *     cmp
 *     jz END
 *     ... # stmtSize
 *     jmp START
 * END:
 */
vector<Instruction> CodeGenerator::generateWhileStmt(const AST *root, const string &scopeName) const 
{
    const auto &children = root->getChildren();
    auto insts = generateExpr(children.at(0), scopeName);
    auto stmtInsts = generateStmt(children.at(1), scopeName);

    const int jzOffset = stmtInsts.size() + 2;
    insts.emplace_back("jz", std::to_string(jzOffset));

    appendInstructions(insts, stmtInsts);

    // negative (GOTO)
    const int jmpOffset = -insts.size();
    insts.emplace_back("jmp", std::to_string(jmpOffset));

    return insts;
}

/**
 * @brief Return value is passed by register (acc)
 * @EBNF ReturnStmt -> return [ Expr ] ';'
 */
vector<Instruction> CodeGenerator::generateReturnStmt(const AST *root, const string &scopeName) const 
{
    if (root == nullptr || root->getChildren().empty()) {
        return {};
    }

    return generateExpr(root->getChildren().at(0), scopeName);
}

/**
 * @EBNF Expr -> SimpleExpr | Variable '=' Expr
 */
vector<Instruction> CodeGenerator::generateExpr(const AST *root, const string &scopeName) const 
{
    // std::cout << "-> Return Expr\n";
    const auto &children = root->getChildren();
    if (children.size() == 1) {
        // SimpleExpr
        return generateSimpleExpr(children.at(0), scopeName);
    }

    // AssignmentExpr
    // Note: calculate Expr first, then Variable
    auto insts = generateExpr(children.at(1), scopeName);
    auto assignmentInsts = generateAssign(children.at(0), scopeName);
    appendInstructions(insts, assignmentInsts);

    return insts;
}

/**
 * @EBNF Variable -> id [ '[' Expr ']' ]
 * 
 * @details
 * index a local/global variable
 * START:
 *     ldc index
 *     ld
 * 
 * index an array element
 * START:
 *     ldc index
 *     ld    # acc = ARRAY_BASE
 *     push  # top = acc (store top)
 *     ...   # acc = ARRAY_OFFSET
 *     add   # acc = ARRAY_BASE + ARRAY_OFFSET
 *     pop   # reserve top
 *     absld # index array element
 */
vector<Instruction> CodeGenerator::generateLoadVariable(const AST *root, const string &scopeName) const 
{
    vector<Instruction> insts;
    const auto &children = root->getChildren();
    const auto &scopeSymbolTable = symbolTable.at(scopeName);
    const auto &globalSymbolTable = symbolTable.at(GLOBAL_SCOPE_NAME);

    const string &id = children.at(0)->getTokenStr();
    if (scopeSymbolTable.find(id) != scopeSymbolTable.end()) {
        // find id in local symbol table

        // load index then load local AR
        insts.emplace_back("ldc", std::to_string(scopeSymbolTable.at(id).variableIndex));
        insts.emplace_back("ld");
    } else if (globalSymbolTable.find(id) != globalSymbolTable.end()) {
        // find id in global symbol table

        insts.emplace_back("ldc", std::to_string(globalSymbolTable.at(id).variableIndex));
        insts.emplace_back("absld");  // FIX BUG!!
    } else {
        throwIdNotFoundErr(id);
    }

    // array element, do more
    if (children.size() == 2) {
        auto exprInsts = generateExpr(children.at(1), scopeName);

        insts.emplace_back("push");

        appendInstructions(insts, exprInsts);

        insts.emplace_back("add");
        insts.emplace_back("pop");
        insts.emplace_back("absld");
    }

    return insts;
}

/**
 * @EBNF SimpleExpr -> AddExpr [ RelationOp AddExpr ]
 * 
 * @details Expression evaluation. 
 * Postorder traverse the binary tree.
 *      <
 *    /   \
 *   3     4
 * 
 * START:
 *     ...   # acc = lhs
 *     push  # top = lhs (store top)
 *     ...   # acc = rhs
 *     op    # acc = top op acc = lhs op rhs
 *     pop   # reserve top
 */
vector<Instruction> CodeGenerator::generateSimpleExpr(const AST *root, const string &scopeName) const
{
    const auto &children = root->getChildren();
    if (children.size() == 1) {
        // std::cout << "-> Return Expr\n";
        return generateAddExpr(children.at(0), scopeName);
    }

    auto insts = generateAddExpr(children.at(0), scopeName);
    auto op = generateRelationOp(children.at(1), scopeName);
    auto rhs = generateAddExpr(children.at(2), scopeName);

    insts.emplace_back("push");
    appendInstructions(insts, rhs);
    appendInstructions(insts, op);
    insts.emplace_back("pop");

    return insts;
}

/**
 * @EBNF RelationOp -> < | <= | > | >= | == | !=
 */
vector<Instruction> CodeGenerator::generateRelationOp(const AST *root, const string &) const 
{
    switch (root->getTokenType()) {
        case TokenType::LESS:
            return { Instruction("lt") };
        case TokenType::LESS_EQUAL:
            return { Instruction("lte") };
        case TokenType::GREATER:
            return { Instruction("gt") };
        case TokenType::GREATER_EQUAL:
            return { Instruction("gte") };
        case TokenType::EQUAL:
            return { Instruction("eq") };
        case TokenType::NOT_EQUAL:
            return { Instruction("neq") };
        default:
            throw std::runtime_error("Invalid token while generating RelationOp");
    }
}

/**
 * @EBNF AddExpr -> Term { AddOp Term }
 * @brief postorder eval, left associative
 */
vector<Instruction> CodeGenerator::generateAddExpr(const AST *root, const string &scopeName) const
{
    const auto &children = root->getChildren();

    // // Debug Only
    // // std::cout << "-> DEBUG\n";
    // for (const auto &child : children) {
    //     // std::cout << child->getLineNo() << " ";
    // }
    // // std::cout << children.size() << ", END DEBUG\n";

    auto insts = generateTerm(children.at(0), scopeName);

    for (int i = 1; i < children.size(); i += 2) {
        auto op = generateAddOp(children.at(i), scopeName);
        auto rhs = generateTerm(children.at(i + 1), scopeName);

        insts.emplace_back("push");
        appendInstructions(insts, rhs);
        appendInstructions(insts, op);
        insts.emplace_back("pop");
    }

    return insts;
}

/**
 * @EBNF AddOp -> + | -
 */
vector<Instruction> CodeGenerator::generateAddOp(const AST *root, const string &scopeName) const 
{
    if (root->getTokenType() == TokenType::PLUS) {
        return { Instruction("add") };
    } else if (root->getTokenType() == TokenType::MINUS) {
        return { Instruction("sub") };
    } else {
        throw std::runtime_error("Invalid token while generating AddOp");
    }
}

/**
 * @EBNF Term -> Factor { MulOp Factor }
 */
vector<Instruction> CodeGenerator::generateTerm(const AST *root, const string &scopeName) const
{
    const auto &children = root->getChildren();

        // Debug Only
    // // std::cout << "-> DEBUG\n";
    // for (const auto &child : children) {
    //     // std::cout << child->getLineNo() << " ";
    // }
    // // std::cout << children.size() << ", END DEBUG\n";

    auto insts = generateFactor(children.at(0), scopeName);

    for (int i = 1; i < children.size(); i += 2) {
        auto op = generateMulOp(children.at(i), scopeName);
        auto rhs = generateFactor(children.at(i + 1), scopeName);

        insts.emplace_back("push");
        appendInstructions(insts, rhs);
        appendInstructions(insts, op);
        insts.emplace_back("pop");
    }

    return insts;
}

/**
 * @EBNF MulOp -> * | /
 */
vector<Instruction> CodeGenerator::generateMulOp(const AST *root, const string &scopeName) const 
{
    if (root->getTokenType() == TokenType::MULTIPLY) {
        return { Instruction("mul") };
    } else if (root->getTokenType() == TokenType::DIVIDE) {
        return { Instruction("div") };
    } else {
        throw std::runtime_error("Invalid token while generating MulOp");
    }    
}

/**
 * @EBNF Factor -> '(' Expr ')' | Variable | Call | literal
 */
vector<Instruction> CodeGenerator::generateFactor(const AST *root, const string &scopeName) const 
{
    if (root == nullptr) {
        return {};
    }
    // std::cout << "-> Trace\n";

    switch (root->getTokenType()) {
        case TokenType::EXPR:
            return generateExpr(root, scopeName);
        case TokenType::VARIABLE:
            return generateLoadVariable(root, scopeName);
        case TokenType::CALL:
            return generateCall(root, scopeName);
        case TokenType::LITERAL:
            return generateLiteral(root, scopeName);
        default:
            throw std::runtime_error("Invalid token while generating Factor");
    }
}

/**
 * @EBNF Call -> id '(' [ ArgList ] ')'
 */
vector<Instruction> CodeGenerator::generateCall(const AST *root, const string &scopeName) const 
{
    const auto &children = root->getChildren();
    const string &id = children.at(0)->getTokenStr();

    // natives
    if (id == "input") {
        return { Instruction("in") };
    } else if (id == "output") {
        auto insts = generateExpr(children.at(1)->getChildren().at(0), scopeName);

        insts.emplace_back("out");
        return insts;
    }

    vector<Instruction> insts;

    if (symbolTable.find(id) == symbolTable.end()) {
        throwIdNotFoundErr(id);
    }
    const auto &scopeSymbolTable = symbolTable.at(id);
    const int n = scopeSymbolTable.size();

    /* 1. caller push paramters and local variables for callee */
    // std::cout << "Call 1\n";

    // LocalVariable n, n - 1, ... 0, Parameter m, m - 1, 0
    vector<pair<string, VariableAttribute>> stackSeq(n);
    for (const auto &[k, v] : scopeSymbolTable) {
        stackSeq.at(n - v.variableIndex - 1) = {k, v};
    }

    int paramSize = 0;
    const bool hasParam = children.size() >= 2;
    if (hasParam) {
        // has param(s)
        paramSize = children.at(1)->getChildren().size();
    }
    const int localVariableSize = n - paramSize;
    // std::cout << localVariableSize << "\n";
    if (localVariableSize < 0)
    {
        throw std::runtime_error((
            boost::format("param size %d too large!") % localVariableSize)
        .str());
    }
    // std::cout << "before push local variables\n";

    // push local variables
    for (int i = 0; i < localVariableSize; i++) {
        const int varSize = stackSeq.at(i).second.varSize;
        if (varSize == 0) {
            // is single
            insts.emplace_back("push");
        } else {
            // is array
            int repeatCnt = varSize;
            while (repeatCnt--) {
                // give array space
                insts.emplace_back("push");
            }

            // calculate the start pointer of array
            insts.emplace_back("addr", std::to_string(varSize));
            // then push it into the stack (write it by acc)
            insts.emplace_back("push");
        }
    }
    // std::cout << "after push local variables\n";

    // push params
    // if a param is an array, it's passed by pointer.
    if (hasParam) {
        auto argListInsts = generateArgList(children.at(1), scopeName);
        appendInstructions(insts, argListInsts);
    }

    /* 2. Caller store context and move PC */

    // Linker will translate the id to an offset.
    insts.emplace_back("call", id);

    /* 3. Caller recollects stack frame used by callee */

    for (const auto &[k, v] : scopeSymbolTable) {
        insts.emplace_back("pop");
        int repeatCnt = v.varSize;
        while (repeatCnt--) {
            insts.emplace_back("pop");
        }
    }
    // std::cout << "All over\n";

    return insts;
}

// vector<Instruction> CodeGenerator::generateNativeCall(const AST *root, const string &scopeName) const {}

/**
 * @EBNF ArgList -> Expr { ',' Expr }
 */
vector<Instruction> CodeGenerator::generateArgList(const AST *root, const string &scopeName) const 
{
    const auto &children = root->getChildren();
    vector<Instruction> insts;

    for (int i = children.size() - 1; i >= 0; i--) {
        auto exprInsts = generateExpr(children.at(i), scopeName);

        appendInstructions(insts, exprInsts);
        insts.emplace_back("push");
    }

    return insts;
}

/**
 * @EBNF Variable -> id [ '[' Expr ']' ]
 * 
 * @details The assignment statement doesn't return a value.
 * before calling this function, the assigned value is stored
 * in acc register.
 */
vector<Instruction> CodeGenerator::generateAssign(const AST *root, const string &scopeName) const 
{
    const auto &children = root->getChildren();
    vector<Instruction> insts;

    insts.emplace_back("push"); // store top = acc

    const auto &scopeSymbolTable = symbolTable.at(scopeName);
    const auto &globalSymbolTable = symbolTable.at(GLOBAL_SCOPE_NAME);
    const string &id = children.at(0)->getTokenStr();

    if (scopeSymbolTable.find(id) != scopeSymbolTable.end()) {
        // find id in local symbol table
        insts.emplace_back("ldc", std::to_string(scopeSymbolTable.at(id).variableIndex));

        if (children.size() == 1) {
            // single
            insts.emplace_back("st");
        } else {
            // array
            auto exprInsts = generateExpr(children.at(1), scopeName);

            insts.emplace_back("ld");  // acc = &array
            insts.emplace_back("push"); // top = &array

            appendInstructions(insts, exprInsts); // acc = offset

            insts.emplace_back("add");  // acc = &(array + offset)
            insts.emplace_back("pop");  // reserve top
            insts.emplace_back("absst");
        }
    } else if (globalSymbolTable.find(id) != globalSymbolTable.end()) {
        insts.emplace_back("ldc", std::to_string(globalSymbolTable.at(id).variableIndex));

        if (children.size() == 1) {
            insts.emplace_back("absst");
        } else {
            auto exprInsts = generateExpr(children.at(1), scopeName);
            insts.emplace_back("absld");
            insts.emplace_back("push");

            appendInstructions(insts, exprInsts); // acc = offset

            insts.emplace_back("add");  // acc = &(array + offset)
            insts.emplace_back("pop");  // reserve top
            insts.emplace_back("absst");
        }
    } else {
        throwIdNotFoundErr(id);
    }

    insts.emplace_back("pop"); // reserve top
    return insts;
}

/**
 * @brief PUSH all global variables into VM stack
 * @details global variables are initialized to zero, by ldc 0.
 */
vector<Instruction> CodeGenerator::generateGlobalVariables() const
{
    vector<Instruction> insts;

    // { ScopeName: { VariableName: VariableAttribute } }
    for (const auto &[_, v] : symbolTable.at(GLOBAL_SCOPE_NAME)) {
        // is array
        if (v.varSize > 0) {
            const int arrayBase = v.variableIndex + 1;
            insts.emplace_back("ldc", std::to_string(arrayBase));
        }
        insts.emplace_back("push");

        int repeatCnt = v.varSize;
        insts.emplace_back("ldc", "0");  // init to zero
        while (repeatCnt--) {
            insts.emplace_back("push");
        }
    }
    return insts;
}

/**
 * @brief automatically call main()
 * @see CodeGenerator::generateCall()
 */
vector<Instruction> CodeGenerator::generateCallMain() const 
{
    vector<Instruction> insts;

    // std::cout << "Test'\n";

    const auto &mainSymbolTable = symbolTable.at(MAIN_NAME);
    const int n = mainSymbolTable.size();
    vector<pair<string, VariableAttribute>> stackSeq(n);


    // local vars
    for (const auto &[k, v] : mainSymbolTable) {
        stackSeq.at(n - v.variableIndex - 1) = {k, v};
    }

    // std::cout << "Set\n";

    // push local vars
    for (const auto &[_, v] : stackSeq) {
        if (v.varSize == 0) {
            insts.emplace_back("push");
        } else {
            int repeatCnt = v.varSize;
            while (repeatCnt--) {
                insts.emplace_back("push");
            }
            insts.emplace_back("addr", std::to_string(v.varSize));
            insts.emplace_back("push");
        }
    }

    insts.emplace_back("call", MAIN_NAME);

    return insts;
}

/**
 * @brief Generate instructions before entering main()
 */
vector<Instruction> CodeGenerator::generateBeforeMain() const 
{
    auto insts = generateGlobalVariables();
    // std::cout << "after gv\n";
    auto instsBeforeMain = generateCallMain();

    appendInstructions(insts, instsBeforeMain);

    return insts;
}

/**
 * @brief Entry of generating instrucions.
 * Then insert ret after all function code.
 * 
 * @return { FunctionId: Instructions } map.
 */
InstructionMap CodeGenerator::createInstMap() const 
{
    // std::cout << "createIM\n";
    InstructionMap instMap {
        {GLOBAL_SCOPE_NAME, generateBeforeMain()}
    };

    for (const AST *const decl : root->getChildren()) {
        auto children = decl->getChildren();
        if (decl->getTokenType() == TokenType::FUNC_DECL) {

            // FuncDecl -> Type id '(' Params ')' CompoundStmt
            const auto &scopeName = children.at(1)->getTokenStr();

            // std::cout << "in for loop\n";

            // CompoundStmt -> '{' LocalVariableDecl StmtList '}'
            auto insts = generateStmtList(children.at(3)->getChildren().at(1), scopeName);

            if (scopeName != MAIN_NAME) {
                insts.emplace_back("ret");
            }
            
            instMap[scopeName] = insts;
        }
    }

    return instMap;
}

/**
 * @brief Reorder and merge the instructions all together, 
 * generate funcCallMap at th same time
 * 
 * @param instMap generated by CodeGenerator::createInstMap()
 * @param funcCallMap An empty map.
 * @return vector<Instruction> all instruction in the program
 * @return-by-reference unordered_map<string, int> &funcCallMap
 *     map functionId to instOffset.
 */
vector<Instruction> CodeGenerator::mergeInstMap(const InstructionMap &instMap, unordered_map<string, int> &funcCallMap)
{
    vector<Instruction> insts;
    appendInstructions(insts, instMap.at(GLOBAL_SCOPE_NAME));

    int instOffset = instMap.at(GLOBAL_SCOPE_NAME).size();

    for (const auto &[k, v] : instMap) {
        if (k != GLOBAL_SCOPE_NAME && k != MAIN_NAME) {
            appendInstructions(insts, v);

            funcCallMap[k] = instOffset;
            instOffset += v.size();
        }
    }

    // append main code at last
    appendInstructions(insts, instMap.at(MAIN_NAME));
    funcCallMap[MAIN_NAME] = instOffset;

    return insts;
}

/**
 * @brief Linker that transform ID call to OFFSET call
 * 
 * @param insts generated by CodeGenerator::mergeInstMap
 * @param funcCallMap generated by CodeGenerator::mergeInstMap
 */
void CodeGenerator::translateCall(vector<Instruction> &insts, const unordered_map<string, int> &funcCallMap)
{
    // linear scan
    for (int i = 0; i < insts.size(); i++) {
        if (insts.at(i).opcodeStr == "call") {
            const int target = funcCallMap.at(insts.at(i).operandStr);
            const int offset = target - i;
            insts.at(i).operandStr = std::to_string(offset);
        }
    }
}

