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

class SyntaxNodeBase{
public:
    bool IsDependOnVars(std::set<SymVar*>&);
    virtual bool IsAffectToVar(SymVar*);
    virtual bool IsDependOnVar(SymVar*);
    virtual bool IsHaveSideEffect();    
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(std::ostream& o, int offset = 0);
    virtual void GetAllAffectedVars(VarsContainer&);
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
