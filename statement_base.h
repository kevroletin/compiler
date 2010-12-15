#ifndef STATEMENT_BASE
#define STATEMENT_BASE

#include "scanner.h"
#include "generator.h"
#include <vector>

class NodeStatement{
public:
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Generate(AsmCode& asm_code);
};

#endif
