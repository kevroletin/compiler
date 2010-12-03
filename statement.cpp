#include "statement.h"

//---NodeStatement---

//---StmtAssgn---

StmtAssign::StmtAssign(Token& op, SyntaxNode* left_, SyntaxNode* right_):
    left(left_),
    right(right_)
{
    if (left->GetSymType() != right->GetSymType()) Error("incompatible types", op);
    if (!(left->IsLValue())) Error("l-value expected", op);
}

const SyntaxNode* StmtAssign::GetLeft() const
{
    return left;
}

const SyntaxNode* StmtAssign::GetRight() const
{
    return right;
}

void StmtAssign::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << ":= \n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

//---StmtBlock---

void StmtBlock::AddStatement(SyntaxNode* new_stmt)
{
    statements.push_back(new_stmt);
}

void StmtBlock::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "begin\n";
    for (vector<SyntaxNode*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        (*it)->Print(o, offset + 1);
    PrintSpaces(o, offset);
    o << "end";
}


//---StmtExpression---

StmtExpression::StmtExpression(SyntaxNode* expression):
    expr(expression)
{
}

void StmtExpression::Print(ostream& o, int offset) const
{
    expr->Print(o, offset);
}
