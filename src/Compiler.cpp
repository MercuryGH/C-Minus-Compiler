#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include "frontend/Lexer.h"
#include "frontend/Parser.h"
#include "frontend/SemanticAnalyzer.h"
#include "backend/CodeGenerator.h"
#include "backend/AssemblyFileIO.h"
#include "ast_vis/AstDumper.h"
#include "Compiler.h"

const string Compiler::WELCOME_PROMPT = "Compiler for C-Minus Programming Language. \nOptions";

/**
 * @brief Read program arguments 
 * 
 * @return true if valid argument provided.
 * @return false if no need to run, or invalid argument.
 */
bool Compiler::readArgs(int argc, char **argv) {
    namespace bpo = boost::program_options;

    bpo::options_description desc(WELCOME_PROMPT);
    desc.add_options()
        ("help,h", "Show help message.")
        (",i", bpo::value<string>(&srcFilePath), "Compile C-Minus source file from <arg> path.")
        (",o", bpo::value<string>(&outputFilePath)->default_value("out.s"), "Output assembly file into <arg> path.")
        (",v", bpo::value<string>(&visualizeAstFilePath), "Visualize AST. Output the serialized AST JSON file into <arg> path.");

    bpo::variables_map var_map;

    try {
        bpo::store(bpo::parse_command_line(argc, argv, desc), var_map);

        // src file is required
        if (var_map.find("help") != var_map.end()) {
            std::cout << desc << "\n";
            return false;
        }

        bpo::notify(var_map);
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "Unknown error during readArgs! \n";
        return false;
    }

    if (srcFilePath.empty()) {
        std::cout << desc << "\n";
        return false;
    }

    return true;
}

void Compiler::compile() const {
    if (srcFilePath.empty() == false) {
        Lexer lexer(srcFilePath);
        auto tokens = lexer.lexicalAnalysis();

        std::cout << "[√] Lexing Complete!\n";

        Parser parser(tokens);
        auto astRoot = parser.syntaxAnalysis();

        if (visualizeAstFilePath.empty() == false) {
            Json::Value json = AstDumper::dump(astRoot);
            std::ofstream writeJson(visualizeAstFilePath);
            writeJson << json;
        }

        std::cout << "[√] Parsing Complete!\n";

        SemanticAnalyzer semanticAnalyzer(astRoot);
        semanticAnalyzer.semanticAnalysis();
        const auto &symbolTable = semanticAnalyzer.getSymbolTable();

        std::cout << "[√] Semantic Analysis Complete!\n";

        // for (const auto &[k, v] : symbolTable) {
        //     std::cout << k << ": ";
        //     for (const auto &[k1, v1] : v) {
        //         std::cout << k1 << ": {";
        //         std::cout << v1.variableIndex << " "
        //                   << v1.variableType << " "
        //                   << v1.varSize << "}\n";
        //     }
        //     std::cout << "\n";
        // }

        CodeGenerator codeGenerator(astRoot, symbolTable);
        auto insts = codeGenerator.generate();

        std::cout << "[√] Generate Code Complete!\n";

        AssemblyFileIO::writeAsmFile(outputFilePath, insts);

        std::cout << "[√] Write Assembly File Complete!\n";


        AST::destroyChildrenRecursively(astRoot);
        delete astRoot;
    } else {
        std::cerr << "Fatal error: no input files.\n";
    }
}

