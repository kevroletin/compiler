#include "syn_table.h"

const string SymbolClassDescription[]{
    "SYM",
    "SYM_FUNCT",
    "SYM_PROC",
    "SYM_TYPE",
    "SYM_TYPE_SCALAR",
    "SYM_TYPE_INTEGER",
    "SYM_TYPE_REAL",
    "SYM_TYPE_ARRAY",
    "SYM_VAR",
};

static void PrintSpaces(ostream& o, int count = 0)
{
    for (int i = 0; i < count; ++i) o << ' ';
}

//---Symbol---

Symbol::Symbol(Token token_):
    token(token_)
{
}

Symbol::Symbol(const Symbol& sym):
    token(sym.token)
{
}

const char* Symbol::GetName() const
{
    return token.GetName();
}

SymbolClass Symbol::GetClassName() const
{
    return SYM;
}

void Symbol::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << token.GetName() << '\t';
}

//---SymType---

SymType::SymType(Token name):
    Symbol(name)
{
}

SymbolClass SymType::GetClassName() const
{
    return SYM_TYPE;
}

void SymType::Print(ostream& o, int offset) const
{
    o << token.GetName() << '\t';
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

SymVar::SymVar(Token token, SymType* type_):
    Symbol(token),
    type(type_)
{
}

SymbolClass SymVar::GetClassName() const
{
    return SYM_VAR;
}

void SymVar::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << token.GetName() << ": ";
    type->Print(o, offset);
}

//---SymTypeScalar---

SymTypeScalar::SymTypeScalar(Token name):
    SymType(name)
{
}

SymbolClass SymTypeScalar::GetClassName() const
{
    return SYM_TYPE_SCALAR;
}

//---SymTypeInteger---

SymTypeInteger::SymTypeInteger(Token name):
    SymTypeScalar(name)
{
}

SymbolClass SymTypeInteger::GetClassName() const
{
    return SYM_TYPE_INTEGER;
}

//---SymTypeFloat---

SymTypeReal::SymTypeReal(Token name):
    SymTypeScalar(name)
{
}

SymbolClass SymTypeReal::GetClassName() const
{
    return SYM_TYPE_REAL;
}

//---SymTypeArray

SymTypeArray::SymTypeArray(Token name, SymType* elem_type_, int low_, int hight_):
    SymType(name),
    elem_type(elem_type_),
    low(low_),
    hight(hight_)
{
}

SymbolClass SymTypeArray::GetClassName() const
{
    return SYM_TYPE_ARRAY;
}

void SymTypeArray::Print(ostream& o, int offset) const
{
    //PrintSpaces(o, offset);
    o << token.GetName() << '[' << low << ".." << hight << "] of ";
    elem_type->Print(o);
}

//---SymTypeRecord

SymTypeRecord::SymTypeRecord(Token name, SynTable* syn_table_):
    SymType(name),
    syn_table(syn_table_)
{
}

void SymTypeRecord::Print(ostream& o, int offset) const
{
    //PrintSpaces(o, offset);
    o << token.GetName() << "\n";
    syn_table->Print(o, offset + 1);
}

//---SynTable

void SynTable::Add(Symbol* sym)
{
    table.insert(sym);
}

#include <iostream>

const Symbol* SynTable::Find(Symbol* sym) const
{
    //std::cout << "------------\n";
    //Print(std::cout);
    //std::cout << (table.find(sym) == table.end()) << "\n";
    Symbol* res = table.find(sym) != table.end() ? *table.find(sym) : NULL;
    return res;
}

const Symbol* SynTable::Find(const Token& tok) const
{
    Symbol sym(tok);
    Symbol* res = table.find(&sym) != table.end() ? *table.find(&sym) : NULL; // fix
    return res;
}

void SynTable::Print(ostream& o, int offset) const
{
    for (std::set<Symbol*, SymbLessComp>::iterator it = table.begin(); it != table.end(); ++it)
    {
        PrintSpaces(o, offset);
//        o << SymbolClassDescription[(*it)->GetClassName()] << "\t\t";
        (*it)->Print(o, offset);
        o << '\n';
    }
}
