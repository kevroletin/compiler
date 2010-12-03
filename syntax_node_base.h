#ifndef SYNTAX_NODE_BASE
#define SYNTAX_NODE_BASE

#include <iostream>
#include "scanner.h"

class SymType;

class SyntaxNode{
public:
    virtual void Print(std::ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    virtual bool IsLValue() const;  
};

extern void Error(string msg, Token token);
extern void PrintSpaces(ostream& o, int offset);

#endif
