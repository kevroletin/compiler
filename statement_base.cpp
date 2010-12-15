#include "statement_base.h"

//---NodeStatement---

void NodeStatement::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << ";\n";
}

void NodeStatement::Generate(AsmCode& asm_code)
{
}
