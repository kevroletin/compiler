#ifndef STATEMENT
#define STATEMENT

#include "statement_base.h"
#include "syntax_node_base.h"
#include "sym_table.h"
#include "scanner.h"
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
    virtual void Generate(AsmCode& asm_code);
};
 
class StmtBlock: public NodeStatement{
private:
    std::vector<NodeStatement*> statements;
public:
    void AddStatement(NodeStatement* new_stmt);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtExpression: public NodeStatement{
private:
    SyntaxNode* expr;
public:
    StmtExpression(SyntaxNode* expression);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtLoop: public NodeStatement{
protected:
    AsmImmidiate break_label;
    AsmImmidiate continue_label;
    NodeStatement* body;
    void ObtainLabels(AsmCode& asm_code);
public:
    StmtLoop(NodeStatement* body_);
    AsmImmidiate GetBreakLabel() const;
    AsmImmidiate GetContinueLabel() const;
    void AddBody(NodeStatement* body);
};

class StmtFor: public StmtLoop{
private:
    const SymVar* index;
    SyntaxNode* init_val;
    SyntaxNode* last_val;
    bool inc;
public:
    StmtFor(const SymVar* index_, SyntaxNode* init_value, SyntaxNode* last_value, bool is_inc, NodeStatement* body_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtWhile: public StmtLoop{
private:
    SyntaxNode* condition;
public:
    StmtWhile(SyntaxNode* condition_, NodeStatement* body_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtUntil: public StmtLoop{
private:
    SyntaxNode* condition;
public:
    StmtUntil(SyntaxNode* condition_, NodeStatement* body = NULL);
    void AddCondition(SyntaxNode* condition);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtIf: public NodeStatement{
private:
    SyntaxNode* condition;
    NodeStatement* then_branch;
    NodeStatement* else_branch;
public:
    StmtIf(SyntaxNode* condition_, NodeStatement* then_branch_, NodeStatement* else_branch_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtJump: public NodeStatement{
private:
    StmtLoop* loop;
    Token op;
public:
    StmtJump(Token tok, StmtLoop* loop_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

class StmtExit: public NodeStatement{
private:
    AsmImmidiate label;
public:
    StmtExit(AsmImmidiate exit_label);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

#endif
