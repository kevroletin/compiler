#ifndef STATEMENT_BASE
#define STATEMENT_BASE

#include "scanner.h"
#include "generator.h"
#include "syntax_node_base.h"
#include <vector>

class SymVar;

enum StmtClassName{
    STMT,
    STMT_ASSIGN,
    STMT_BLOCK,
    STMT_EXPRESSION,
    STMT_LOOP,
    STMT_IF,
    STMT_JUMP,
    STMT_EXIT
};

class NodeStatement: public SyntaxNodeBase{
public:
    virtual StmtClassName GetClassName() const;
    virtual void Generate(AsmCode& asm_code);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset);
    virtual void Optimize();
};

#endif
