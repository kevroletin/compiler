#ifndef STATEMENT_BASE
#define STATEMENT_BASE

#include "scanner.h"
#include "generator.h"
#include "syntax_node_base.h"
#include <vector>

class SymVar;

class NodeStatement: public SyntaxNodeBase{
public:
    virtual void Generate(AsmCode& asm_code);
};

#endif
