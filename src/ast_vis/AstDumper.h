#pragma once

#include <json/json.h>
#include "AST.h"

class AstDumper
{
public:
    static Json::Value dump(const AST *root);
};
