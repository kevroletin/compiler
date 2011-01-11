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
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual StmtClassName GetClassName() const;
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
};

class StmtBlock: public NodeStatement{
protected:
    std::vector<NodeStatement*> statements;
public:
    virtual void Optimize();
    void OptimizeLoops();
    void OptimizeConstants();
    NodeStatement* GetStmt(unsigned i);
    bool IsEmpty() const;
    unsigned GetSize() const;
    void AddStatement(NodeStatement* new_stmt);
    void CopyContent(StmtBlock* src);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var) const;
    virtual bool IsHaveSideEffect() const;
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
    virtual StmtClassName GetClassName() const;
};

class StmtExpression: public NodeStatement{
private:
    SyntaxNode* expr;
public:
    StmtExpression(SyntaxNode* expression);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
    virtual StmtClassName GetClassName() const;
    virtual bool CanBeReplaced();
};

class StmtLoop: public NodeStatement{
protected:
    AsmStrImmediate break_label;
    AsmStrImmediate continue_label;
    StmtBlock* body;
    void ObtainLabels(AsmCode& asm_code);
    virtual void CalculateDependences(set<SymVar*>& affectte_cont, set<SymVar*>& deps);
public:
    StmtBlock* GetBody() const;
    void TakeOutVars(std::vector<NodeStatement*>& before_loop);
    StmtLoop(NodeStatement* body_);
    AsmStrImmediate GetBreakLabel() const;
    AsmStrImmediate GetContinueLabel() const;
    void AddBody(NodeStatement* body);
    virtual StmtClassName GetClassName() const;
    virtual bool IsConditionAffectToVars();
};

class StmtFor: public StmtLoop{
private:
    SymVar* index;
    SyntaxNode* init_val;
    SyntaxNode* last_val;
    bool inc;
    virtual void CalculateDependences(set<SymVar*>& affected_cont, set<SymVar*>& deps);
public:
    StmtFor(SymVar* index_, SyntaxNode* init_value, SyntaxNode* last_value,
            bool is_inc, NodeStatement* body_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
    virtual bool IsConditionAffectToVars();
};

class StmtWhile: public StmtLoop{
protected:
    SyntaxNode* condition;    
    virtual void CalculateDependences(set<SymVar*>& affected_cont, set<SymVar*>& deps);
public:
    StmtWhile(SyntaxNode* condition_ = NULL , NodeStatement* body_ = NULL);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
    virtual bool IsConditionAffectToVars();
};

class StmtUntil: public StmtWhile{
public:
    StmtUntil(SyntaxNode* condition_, NodeStatement* body = NULL);
    void AddCondition(SyntaxNode* condition);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
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
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual void MakeDependencesGraph(DependedVerts& v, DependencyGraph& g);
    virtual StmtClassName GetClassName() const;
    virtual bool CanBeReplaced();
    
};

class StmtJump: public NodeStatement{
private:
    StmtLoop* loop;
    Token op;
public:
    StmtJump(Token tok, StmtLoop* loop_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual StmtClassName GetClassName() const;
    virtual bool CanBeReplaced();
};

class StmtExit: public NodeStatement{
private:
    AsmStrImmediate label;
public:
    StmtExit(AsmStrImmediate exit_label);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void Generate(AsmCode& asm_code);
    virtual bool IsAffectToVar(SymVar* var);
    virtual bool IsDependOnVar(SymVar* var);
    virtual bool IsHaveSideEffect();
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual StmtClassName GetClassName() const;
    virtual bool CanBeReplaced();
};

#endif
