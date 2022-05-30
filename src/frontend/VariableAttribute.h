#pragma once

#include <string>

using std::string;

struct VariableAttribute {
    // use to index variable in the scope
    int variableIndex;  

    // Type::tokenStr
    string variableType;

    // int occupies one size. int array[n] means varSize = n.
    int varSize;

    VariableAttribute():
        variableIndex(-1), 
        variableType(""),
        varSize(0) {}  

    VariableAttribute(int variableIndex, const string &variableType):
        variableIndex(variableIndex), 
        variableType(variableType),
        varSize(0) {}
    
    VariableAttribute(int variableIndex, const string &variableType, int varSize):
        variableIndex(variableIndex), 
        variableType(variableType),
        varSize(varSize) {}
};