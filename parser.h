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
    Scanner& scan;
    SynTable top_syn_table;
    SymType* top_type_bool;
    std::vector<SynTable*> syn_table_stack;
    void NextToken();
    SyntaxNode* GetTerm();
    SyntaxNode* GetAddingExpr();
    SyntaxNode* GetMultiplyingExpr();
    SyntaxNode* GetUnaryExpr();
    SyntaxNode* GetRelationalExpr();
    void Error(string msgn);
    //void PrintNode(ostream& o, Expression* e, int margin = 0);
    SymType* ParseType();
    void ParseConstDefinitions();
    void ParseVarDefinitions();
    void ParseTypeDefinitions();
    void ParseStatement();
    const Symbol* FindSymbol(Symbol* sym);
    const Symbol* FindSymbolOrDie(Symbol* sym, SymbolClass type, string msg);
    const Symbol* FindSymbolOrDie(Token tok, SymbolClass type, string msg); 
    const Symbol* FindSymbol(const Token& tok);
    SyntaxNode* ConvertType(SyntaxNode* node, const SymType* type);
    void TryToConvertType(SyntaxNode*& first, SyntaxNode*& second);
public:
    Parser(Scanner& scanner);
    void Parse();
    void PrintSimpleParse(ostream& o);
    void PrintDeclarationsParse(ostream& o);
};

#endif
