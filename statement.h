#ifndef STATEMENT
#define STATEMENT

#include "statement_base.h"
#include "syntax_node_base.h"
#include "sym_table.h"
#include "exception.h"
#include <vector>
#include <string>

class StmtAssign: public NodeStatement{
private:
    SyntaxNode* left;
    SyntaxNode* right;
public:
    StmtAssign(SyntaxNode* left_, SyntaxNode* right_);
    const SyntaxNode* GetLeft() const;
    const SyntaxNode* GetRight() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};
 
class StmtBlock: public NodeStatement{
private:
    std::vector<NodeStatement*> statements;
public:
    void AddStatement(NodeStatement* new_stmt);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

class StmtExpression: public NodeStatement{
private:
    SyntaxNode* expr;
public:
    StmtExpression(SyntaxNode* expression);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

class StmtFor: public NodeStatement{
private:
    const SymVar* index;
    SyntaxNode* init_val;
    SyntaxNode* last_val;
    bool inc;
    NodeStatement* body;
public:
    StmtFor(const SymVar* index_, SyntaxNode* init_value, SyntaxNode* last_value, bool is_inc, NodeStatement* body_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

class StmtWhile: public NodeStatement{
private:
    SyntaxNode* condition;
    NodeStatement* body;
public:
    StmtWhile(SyntaxNode* condition_, NodeStatement* body_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

class StmtUntil: public NodeStatement{
private:
    SyntaxNode* condition;
    NodeStatement* body;
public:
    StmtUntil(SyntaxNode* condition_, NodeStatement* body);
    virtual void Print(ostream& o, int offset = 0) const;
    void Generate(AsmCode& asm_code) const;
};

class StmtIf: public NodeStatement{
private:
    SyntaxNode* condition;
    NodeStatement* then_branch;
    NodeStatement* else_branch;
public:
    StmtIf(SyntaxNode* condition_, NodeStatement* then_branch_, NodeStatement* else_branch_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

class StmtJump: public NodeStatement{
private:
    AsmImmidiate label;
    string name;
public:
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code) const;
};

#endif
