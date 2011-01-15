#include "syntax_node_base.h"

//---SyntaxNodeBase---

bool SyntaxNodeBase::IsDependOnVars(std::set<SymVar*>& vars)
{
    std::set<SymVar*> t;
    GetAllDependences(t);
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (t.find(*it) != t.end()) return true;
    return false;

    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (IsDependOnVar(*it)) return true;
    return false;
}

bool SyntaxNodeBase::IsAffectToVars(std::set<SymVar*>& vars)
{
    std::set<SymVar*> t;
    GetAllAffectedVars(t);
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (t.find(*it) != t.end()) return true;
    return false;

    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (IsAffectToVar(*it)) return true;
    return false;
}

void SyntaxNodeBase::AddToDependencyGraph(DependedVerts& v, DependencyGraph& g,
                                          SymVar* dep_sym, SyntaxNodeBase* expr)
{
    std::set<SymVar*> t;
    expr->GetAllDependences(t);
    for (std::set<SymVar*>::iterator it= t.begin(); it != t.end(); ++it)
    {
        v.insert(*it);
        g[*it].insert(dep_sym);
    }
}

void SyntaxNodeBase::AddToDependencyGraph(DependedVerts& v, DependencyGraph& g,
                                          SymVar* dep_sym, set<SymVar*>& src)
{
    for (std::set<SymVar*>::iterator it= src.begin(); it != src.end(); ++it)
    {
        v.insert(*it);
        g[*it].insert(dep_sym);
    }
}

void SyntaxNodeBase::AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, set<SymVar*>& depended, set<SymVar*>& src)
{
    for (set<SymVar*>::iterator it = depended.begin(); it != depended.end(); ++it)
        AddToDependencyGraph(v, g, *it, src);
}

void SyntaxNodeBase::AddToDependencyGraph(DependedVerts& v, DependencyGraph& g,
                                          SyntaxNodeBase* depended, set<SymVar*>& src)
{
    std::set<SymVar*> t;
    depended->GetAllAffectedVars(t);
    for (std::set<SymVar*>::iterator it= t.begin(); it != t.end(); ++it)
    {
        AddToDependencyGraph(v, g, *it, src);
    }
}

void SyntaxNodeBase::AddToDependencyGraph(DependedVerts& v, DependencyGraph& g,
                                          SyntaxNodeBase* depended, SyntaxNodeBase* expr)
{
    std::set<SymVar*> t;
    depended->GetAllAffectedVars(t);
    for (std::set<SymVar*>::iterator it= t.begin(); it != t.end(); ++it)
    {
        AddToDependencyGraph(v, g, *it, expr);
    }
}

void SyntaxNodeBase::ComputeAllDependences(DependedVerts& v, DependencyGraph& g)
{
    for (set<SymVar*>::iterator a = v.begin(); a != v.end(); ++a)
        for (set<SymVar*>::iterator b = v.begin(); b != v.end(); ++b)
            for (set<SymVar*>::iterator c = v.begin(); b != v.end(); ++b)
            {
                if (g[*a].find(*b) != v.end() && g[*b].find(*c) != v.end())
                    g[*a].insert(*c);
            }
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

void SyntaxNodeBase::GetAllDependences(VarsContainer&, bool with_self)
{
}

bool SyntaxNodeBase::CanBeReplaced()
{
    return true;
}

void SyntaxNodeBase::MakeDependencesGraph(DependedVerts& v, DependencyGraph& g)
{
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
    


