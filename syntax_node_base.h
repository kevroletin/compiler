#ifndef SYNTAX_NODE_BASE
#define SYNTAX_NODE_BASE

#include "scanner.h"
#include "generator.h"
#include <ostream>
#include <set>

class SymType;
class SymVar;

extern SymType* top_type_int;
extern SymType* top_type_real;
extern SymType* top_type_untyped;
extern SymType* top_type_str;

typedef std::set<SymVar*> VarsContainer;
typedef std::map<SymVar*, std::set<SymVar*> > DependencyGraph;
typedef std::set<SymVar*> DependedVerts;

class SyntaxNodeBase{
public:
    bool IsDependOnVars(std::set<SymVar*>& vars);
    bool IsAffectToVars(std::set<SymVar*>& vars);
    void AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, SymVar* dep_sym, SyntaxNodeBase* expr);
    void AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, SymVar* dep_sym, set<SymVar*>& src);
    void AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, SyntaxNodeBase* depended, SyntaxNodeBase* expr);
    void AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, SyntaxNodeBase* depended, set<SymVar*>& src);
    void AddToDependencyGraph(DependedVerts& v, DependencyGraph& g, set<SymVar*>& depended, set<SymVar*>& src);
    void ComputeAllDependences(DependedVerts& v, DependencyGraph& g);
    virtual bool IsAffectToVar(SymVar*);
    virtual bool IsDependOnVar(SymVar*);    
    virtual bool IsHaveSideEffect();    
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(std::ostream& o, int offset = 0);
    virtual void GetAllAffectedVars(VarsContainer&);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual bool CanBeReplaced();
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
};

class SyntaxNode: public SyntaxNodeBase{
public:
    virtual const SymType* GetSymType() const;
    virtual bool IsLValue() const;
    virtual SymVar* GetAffectedVar() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;    
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual bool IsConst() const;
    virtual Token ComputeConstExpr() const;
    virtual int ComputeIntConstExpr() const;
    virtual float ComputeRealConstExpr() const;
    virtual bool TryToBecomeConst(SyntaxNode*& link);
};

#endif
