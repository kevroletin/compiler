#include "syntax_node_base.h"

//---SyntaxNode---

void SyntaxNode::Print(ostream& o, int offset) const 
{
}

const SymType* SyntaxNode::GetSymType() const
{
    return NULL;
}

bool SyntaxNode::IsLValue() const
{
    return false;
}

//---Mix---

void Error(string msg, Token token)
{
    stringstream s;
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}

void PrintSpaces(ostream& o, int offset)
{
    for (int i = 0; i < offset; ++i)
        o << "  ";
}

