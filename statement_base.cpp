#include "statement_base.h"

//---NodeStatement---

void NodeStatement::Generate(AsmCode& asm_code)
{
}

void NodeStatement::Print(ostream& o, int offset) 
{
    PrintSpaces(o, offset) << ";\n";
}

void NodeStatement::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << ";\n";
}
