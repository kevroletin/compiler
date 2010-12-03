#ifndef STATEMENT
#define STATEMENT

#include "syntax_node_base.h"
#include "scanner.h"
#include <vector>

class NodeStatement: public SyntaxNode{
};

class StmtAssign: public NodeStatement{
private:
    SyntaxNode* left;
    SyntaxNode* right;
public:
    StmtAssign(Token& op, SyntaxNode* left_, SyntaxNode* right_);
    const SyntaxNode* GetLeft() const;
    const SyntaxNode* GetRight() const;
    virtual void Print(ostream& o, int offset = 0) const;
};
 
class StmtBlock: public NodeStatement{
private:
    std::vector<SyntaxNode*> statements;
public:
    void AddStatement(SyntaxNode* new_stmt);
    virtual void Print(ostream& o, int offset = 0) const;    
};

class StmtExpression: public NodeStatement{
private:
    SyntaxNode* expr;
public:
    StmtExpression(SyntaxNode* expression);
    virtual void Print(ostream& o, int offset = 0) const;
};

#endif
