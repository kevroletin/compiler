#include "syntax_node_base.h"

//---SyntaxNode---

void SyntaxNode::Print(ostream& o, int offset) const 
{
}

const SymType* SyntaxNode::GetSymType() const
{
    return NULL;
}

bool SyntaxNode::IsLValue() const
{
    return false;
}

void SyntaxNode::GenerateLValue(AsmCode& asm_code) const
{
}

void SyntaxNode::GenerateValue(AsmCode& asm_code) const
{
}

bool SyntaxNode::IsConst() const
{
    return false;
}

Token SyntaxNode::ComputeConstExpr() const
{
    if (GetSymType() == top_type_int) return Token(ComputeIntConstExpr());
    return Token(ComputeRealConstExpr());
}

int SyntaxNode::ComputeIntConstExpr() const
{
    return 0;
}

float SyntaxNode::ComputeRealConstExpr() const
{
    return 0;
}

bool SyntaxNode::TryToBecomeConst(SyntaxNode*& link)
{
    return false;
}


bool SyntaxNode::IsHaveSideEffect() const
{
    return true;
}

bool SyntaxNode::IsAffectToVar(SymVar*) const
{
    return true;
}

    


