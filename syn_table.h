#ifndef SYN_TABLE
#define SYN_TABLE

#include <string.h>
#include "scanner.h"
#include <map>
#include <set>

enum SymbolClass{
    SYM,
    SYM_FUNCT,
    SYM_PROC,
    SYM_TYPE,
    SYM_TYPE_SCALAR,
    SYM_TYPE_INTEGER,
    SYM_TYPE_REAL,
    SYM_TYPE_ARRAY,
    SYM_VAR,
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

};

class SymTypePointer: public SymType{

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
