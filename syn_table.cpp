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
    for (int i = 0; i < count; ++i) o << "  ";
}

//---Symbol---

Symbol::Symbol(Token token_)
{
    token_.NameToLowerCase();
    token = token_;
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
    return SymbolClass(SYM | SYM_TYPE);
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
    return SymbolClass(SYM | SYM_PROC);
}

//---SymFunct---

SymFunct::SymFunct(Token token, SynTable* syn_table, SymType return_type_):
    SymProc(token, syn_table),
    return_type(return_type_)
{
}

SymbolClass SymFunct::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_FUNCT);
}

//---SymVar---

SymVar::SymVar(Token token, SymType* type_):
    Symbol(token),
    type(type_)
{
}

SymbolClass SymVar::GetClassName() const
{
    return SymbolClass(SYM | SYM_VAR);
}

void SymVar::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << token.GetName() << ": ";
    type->Print(o, offset + 1);
}

//---SymTypeScalar---

SymTypeScalar::SymTypeScalar(Token name):
    SymType(name)
{
}

SymbolClass SymTypeScalar::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_SCALAR);
}

//---SymTypeInteger---

SymTypeInteger::SymTypeInteger(Token name):
    SymTypeScalar(name)
{
}

SymbolClass SymTypeInteger::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_SCALAR | SYM_TYPE_INTEGER);
}

//---SymTypeFloat---

SymTypeReal::SymTypeReal(Token name):
    SymTypeScalar(name)
{
}

SymbolClass SymTypeReal::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_SCALAR | SYM_TYPE_REAL);
}

//---SymTypeArray---

SymTypeArray::SymTypeArray(Token name, SymType* elem_type_, int low_, int hight_):
    SymType(name),
    elem_type(elem_type_),
    low(low_),
    hight(hight_)
{
}

SymbolClass SymTypeArray::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_ARRAY);
}

void SymTypeArray::Print(ostream& o, int offset) const
{
    //PrintSpaces(o, offset);
    o << token.GetName() << '[' << low << ".." << hight << "] of ";
    elem_type->Print(o, offset);
}

//---SymTypeRecord---

SymTypeRecord::SymTypeRecord(Token name, SynTable* syn_table_):
    SymType(name),
    syn_table(syn_table_)
{
}

void SymTypeRecord::Print(ostream& o, int offset) const
{
    o << token.GetName() << "\n";
    syn_table->Print(o, offset);
    PrintSpaces(o, offset - 1);
    o << "end";
}

//---SymTypeAlias---

SymTypeAlias::SymTypeAlias(Token name, SymType* target_):
    SymType(name),
    target(target_)
{    
}

void SymTypeAlias::Print(ostream& o, int offset) const
{
    o << target->GetName() << " = " << token.GetName();
}

SymbolClass SymTypeAlias::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_ALIAS);
}

//---SymTypePointer---

SymTypePointer::SymTypePointer(Token name, SymType* ref_type_):
    SymType(name),
    ref_type(ref_type_)
{  
}

void SymTypePointer::Print(ostream& o, int offset) const
{
    o << '^';
    ref_type->Print(o, offset);
}

SymbolClass  SymTypePointer::GetClassName() const
{
    return SymbolClass(SYM | SYM_TYPE | SYM_TYPE_POINTER);
}

//---SynTable---

void SynTable::Add(Symbol* sym)
{    
    table.insert(sym);
}

#include <iostream>

const Symbol* SynTable::Find(Symbol* sym) const
{
    return (table.find(sym) != table.end()) ? *table.find(sym) : NULL;
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
        (*it)->Print(o, offset);
        o << '\n';
    }
}
