#include "syntax_node_base.h"

//---SyntaxNodeBase---

bool SyntaxNodeBase::IsDependOnVars(std::set<SymVar*>& vars)
{
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (IsDependOnVar(*it)) return true;
    return false;
}

bool SyntaxNodeBase::IsAffectToVars(std::set<SymVar*>& vars)
{
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (IsAffectToVar(*it)) return true;
    return false;
}

void SyntaxNodeBase::Print(ostream& o, int offset) 
{
}

void SyntaxNodeBase::Print(ostream& o, int offset) const
{
}

bool SyntaxNodeBase::IsHaveSideEffect()
{
    return true;
}

bool SyntaxNodeBase::IsDependOnVar(SymVar*)
{
    return true;
}

bool SyntaxNodeBase::IsAffectToVar(SymVar*)
{
    return true;
}

void SyntaxNodeBase::GetAllAffectedVars(std::set<SymVar*>&)
{
}

void SyntaxNodeBase::GetAllDependences(VarsContainer&)
{
}

bool SyntaxNodeBase::CanBeReplaced()
{
    return true;
}

//---SyntaxNode---

const SymType* SyntaxNode::GetSymType() const
{
    return NULL;
}

bool SyntaxNode::IsLValue() const
{
    return false;
}

SymVar* SyntaxNode::GetAffectedVar() const
{
    return NULL;
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
    


