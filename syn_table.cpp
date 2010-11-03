#include "syn_table.h"

//---Symbol---

Symbol::Symbol(char* name_):
    name(strcpy(new char[strlen(name_) + 1], name_))
{
}

Symbol::Symbol(const Symbol& sym):
    name(strcpy(new char[strlen(sym.name) + 1], sym.name))
{
}

Symbol::~Symbol()
{
    delete(name);
}

SymbolClass Symbol::GetClassName() const // оч плохо
{
}

const char* Symbol::GetName() const
{
    return name;
}

//---SymType---

SymbolClass SymType::GetClassName() const
{
    return SYM_TYPE;
}

//---SymProc---

SymProc::SymProc(char* name, SynTable* syn_table_):
    Symbol(name),
    syn_table(syn_table_)
{
}

SymbolClass SymProc::GetClassName() const
{
    return SYM_PROC;
}

//---SymFunct---

SymFunct::SymFunct(char* name, SynTable* syn_table, SymType return_type_):
    SymProc(name, syn_table),
    return_type(return_type_)
{
}

SymbolClass SymFunct::GetClassName() const
{
    return SYM_FUNCT;
}

//---SymVar---

SymVar::SymVar(char* name, SymType type_):
    Symbol(name),
    type(type_)
{
}

SymbolClass SymVar::GetClassName() const
{
    return SYM_VAR;
}

//---SynTable

void SynTable::Add(Symbol* sym)
{
    table.insert(sym);
}

const Symbol* SynTable::Find(Symbol* sym) const
{
    Symbol* res = table.find(sym) == table.end() ? *table.find(sym) : NULL;
    return res;
}

