#ifndef SYN_TABLE
#define SYN_TABLE

#include <string.h>
#include <map>
#include <set>

enum SymbolClass{
    SYM_FUNCT,
    SYM_PROC,
    SYM_TYPE,
    SYM_VAR
};

class SynTable;

class Symbol{
private:
    Token token;
public:
    Symbol(Token token_);
    Symbol(const Symbol& sym);
    ~Symbol();
    const char* GetName() const;
    virtual SymbolClass GetClassName() const;
};

class SymType: public Symbol{
public:
    virtual SymbolClass GetClassName() const;
};

class SymProc: public Symbol{
private:
    SynTable* syn_table;
public:
    SymProc(char* name, SynTable* syn_table_);
    virtual SymbolClass GetClassName() const;
};

class SymFunct: public SymProc{
private:
    SymType return_type;
public:
    SymFunct(char* name, SynTable* syn_table, SymType return_type_);
    virtual SymbolClass GetClassName() const ;
};

class SymVar: public Symbol{
private:
    SymType type;
public:
    SymVar(char* name, SymType type_);
    virtual SymbolClass GetClassName() const;
};

//---SymType descendants---

class SymTypeScalar: public SymType{

};

class SymTypeInteger: public SymTypeScalar{

};

class SymTypeFloat: public SymTypeScalar{

};

class SymTypeArray: public SymType{

};

class SymTypeRecord: public SymType{

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

class SymVarGloval: public SymVar{

};

class SymVarLocal: public SymVar{

};

//---SynTable---

class SynTable{
private:
    struct SymbLessComp{
        bool operator () (Symbol* a, Symbol* b) const
        {
            return strcmp(a->GetName(), b->GetName()) < 0;
        }
    };
    std::set<Symbol*, SymbLessComp> table;
public:
    void Add(Symbol* sym);
    const Symbol* Find(Symbol* name) const;
};

#endif
