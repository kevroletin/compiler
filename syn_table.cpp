#include "syn_table.h"

//---Symbol---

Symbol::Symbol(Token token_):
    token(token_)
{
}

Symbol::Symbol(const Symbol& sym):
    token(sym.token)
{
}

SymbolClass Symbol::GetClassName() const // оч плохо
{
}

const char* Symbol::GetName() const
{
    return token.GetName();
}

//---SymType---

SymbolClass SymType::GetClassName() const
{
    return SYM_TYPE;
}

//---SymProc---

SymProc::SymProc(Token token, SynTable* syn_table_):
    Symbol(token),
    syn_table(syn_table_)
{
}

SymbolClass SymProc::GetClassName() const
{
    return SYM_PROC;
}

//---SymFunct---

SymFunct::SymFunct(Token token, SynTable* syn_table, SymType return_type_):
    SymProc(token, syn_table),
    return_type(return_type_)
{
}

SymbolClass SymFunct::GetClassName() const
{
    return SYM_FUNCT;
}

//---SymVar---

SymVar::SymVar(Token token, SymType type_):
    Symbol(token),
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

