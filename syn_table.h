#ifndef SYN_TABLE
#define SYN_TABLE

#include <string.h>
#include "scanner.h"
#include <map>
#include <set>

enum SymbolClass{
    SYM = 0,
    SYM_FUNCT = 1,
    SYM_PROC = 2,
    SYM_TYPE = 4,
    SYM_TYPE_SCALAR = 8,
    SYM_TYPE_INTEGER = 16,
    SYM_TYPE_REAL = 32,
    SYM_TYPE_ARRAY = 64,
    SYM_TYPE_ALIAS = 128,
    SYM_TYPE_POINTER = 256,
    SYM_VAR = 512,
};

class SynTable;

class Symbol{
protected:
    Token token;
public:
    Symbol(Token token_);
    Symbol(const Symbol& sym);
    const char* GetName() const;
    virtual SymbolClass GetClassName() const;
//    virtual ostream& operator<< (ostream& o) const;
    virtual void Print(ostream& o, int offset = 0) const;
};

class SymType: public Symbol{
public:
    SymType(Token token);
    virtual SymbolClass GetClassName() const;
    void Print(ostream& o, int offset = 0) const;
};

class SymProc: public Symbol{
private:
    SynTable* syn_table;
public:
    SymProc(Token token_, SynTable* syn_table_);
    virtual SymbolClass GetClassName() const;
};

class SymFunct: public SymProc{
private:
    SymType return_type;
public:
    SymFunct(Token token_, SynTable* syn_table, SymType return_type_);
    virtual SymbolClass GetClassName() const ;
};

class SymVar: public Symbol{
private:
    SymType* type;
public:
    SymVar(Token token_, SymType* type_);
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
};

//---SymType descendants---

class SymTypeScalar: public SymType{
public:
    SymTypeScalar(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeInteger: public SymTypeScalar{
public:
    SymTypeInteger(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeReal: public SymTypeScalar{
public:
    SymTypeReal(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeArray: public SymType{
private:
    SymType* elem_type;
    int low;
    int hight;
public:
    SymTypeArray(Token name, SymType* type_, int low_, int hight_);
    int GetLow();
    int GetHight();
    SymType GetType();
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
};

class SymTypeRecord: public SymType{
private:
    SynTable* syn_table;
public:
    SymTypeRecord(Token name, SynTable* syn_table_);
    virtual void Print(ostream& o, int offset = 0) const;
};

class SymTypeAlias: public SymType{
private:
    SymType* target;
public:
    SymTypeAlias(Token name, SymType* ratget_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual SymbolClass GetClassName() const;
};

class SymTypePointer: public SymType{
private:
    SymType* ref_type;
public:
    SymTypePointer(Token name, SymType* ref_type_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual SymbolClass GetClassName() const;    
};

//---SymVar descendants---

class SymVarConst: public SymVar{

};

class SymVarParam: public SymVar{

};

class SymVarGlobal: public SymVar{

};

class SymVarLocal: public SymVar{

};

//---SynTable---

#include <iostream>

class SynTable{
private:
    struct SymbLessComp{
        bool operator () (Symbol* a, Symbol* b) const
        {
            //std::cout << "compare: " << a->GetName() << " - " << b->GetName() << " res: " << (strcmp(a->GetName(), b->GetName()) < 0 )<< "\n";
            return strcmp(a->GetName(), b->GetName()) < 0;
        }
    };
    std::set<Symbol*, SymbLessComp> table;
public:
    void Add(Symbol* sym);
    const Symbol* Find(Symbol* sym) const;
    const Symbol* Find(const Token& tok) const;
    void Print(ostream& o, int offset = 0) const;
};

#endif
