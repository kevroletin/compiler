#ifndef PARSER
#define PARSER

#include <stdlib.h>
#include "scanner.h"
#include "syn_table.h"
#include "syntax_node.h"
#include <string.h>
#include "exception.h"
#include <vector>
#include <utility>
#include <stack>

#include <ostream>

class Parser{
private:
    SyntaxNode* syntax_tree;
    Scanner& scan;
    SynTable top_sym_table;
    SymType* top_type_bool;
    std::vector<SynTable*> sym_table_stack;
    void NextToken();
    SyntaxNode* GetTerm();
    SyntaxNode* GetAddingExpr();
    SyntaxNode* GetMultiplyingExpr();
    SyntaxNode* GetUnaryExpr();
    SyntaxNode* GetRelationalExpr();
    void Error(string msgn);
    SymType* ParseType();
    void ParseVarDefinitions(bool is_global = true);
    void ParseTypeDefinitions();
    void ParseFunctionDefinition();
    NodeStatement* ParseStatement();
    const Symbol* FindSymbol(Symbol* sym);
    const Symbol* FindSymbolOrDie(Symbol* sym, SymbolClass type, string msg);
    const Symbol* FindSymbolOrDie(Token tok, SymbolClass type, string msg); 
    const Symbol* FindSymbol(const Token& tok);
    void Parse();
public:
    Parser(Scanner& scanner);
    void PrintSyntaxTree(ostream& o);
    void PrintSymTable(ostream& o);
};

#endif
