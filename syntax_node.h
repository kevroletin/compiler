#ifndef SYNTAX_NODE
#define SYNTAX_NODE
#include "scanner.h"
#include "syn_table.h"
#include "exception.h"
#include <vector>

static void Error(string msg, Token token);

class SyntaxNode{
public:
    static SyntaxNode* ConvertType(SyntaxNode* node, const SymType* type);
    static void TryToConvertType(SyntaxNode*& first, SyntaxNode*& second);
    static void TryToConvertType(SyntaxNode*& expr, const SymType* type);
    static void TryToConvertTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err);
    static void TryToConvertTypeOrDie(SyntaxNode*& expr, const SymType* type, Token tok_err);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    virtual bool IsLValue() const;  
};

class NodeStatement: public SyntaxNode{
protected:
    SyntaxNode* child;
public:
    NodeStatement(SyntaxNode* child_);
    virtual void Print(ostream& o, int offset = 0) const;
};

class StmtAssign: public NodeStatement{
private:
    SyntaxNode* right;
public:
    StmtAssign(Token& op, SyntaxNode* left_, SyntaxNode* right_);
    const SyntaxNode* GetLeft() const;
    const SyntaxNode* GetRight() const;
    virtual void Print(ostream& o, int offset = 0) const;
};

class NodeCall: public SyntaxNode{
private:
    std::vector<SyntaxNode*> args;
    SymFunct* funct;
public: 
    NodeCall(SyntaxNode* epr);
    NodeCall(SymFunct* funct_);
    void AddArg(SyntaxNode* arg);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
};

class NodeBinaryOp: public SyntaxNode{
private:
    Token token;
    SyntaxNode* left;
    SyntaxNode* right;
public:
    NodeBinaryOp(const Token& name, SyntaxNode* left_, SyntaxNode* right_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
};

class NodeUnaryOp: public SyntaxNode{
protected:
    Token token;
    SyntaxNode* child;
public:
    NodeUnaryOp(const Token& name, SyntaxNode* child_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
};

class NodeIntToRealConv: public NodeUnaryOp{
private:
    SymType* real_type;
public:
    NodeIntToRealConv(SyntaxNode* child_, SymType* real_type_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
};

class NodeVar: public SyntaxNode{
private:
    SymVar* var;
public:
    NodeVar(Symbol* var_);
    NodeVar(SymVar* var_);
    const SymVar* GetVar();
    virtual const SymType* GetSymType() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual bool IsLValue() const;
};

class NodeArrayAccess: public SyntaxNode{
private:
    SyntaxNode* arr;
    SyntaxNode* index;
public:
    NodeArrayAccess(SyntaxNode* arr_, SyntaxNode* index_, Token tok);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
    virtual bool IsLValue() const;
};

class NodeRecordAccess: public SyntaxNode{
private:
    const SyntaxNode* record;
    const SymVar* field;
public:
    NodeRecordAccess(SyntaxNode* record_, Token field_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
    virtual bool IsLValue() const;
};

#endif
