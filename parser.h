#ifndef PARSER
#define PARSER

#include <stdlib.h>
#include "scanner.h"
#include "sym_table.h"
#include "syntax_node.h"
#include "exception.h"
#include <string.h>
#include <vector>
#include <utility>
#include <stack>

#include <ostream>

extern SyntaxNode* ConvertType(SyntaxNode* node, const SymType* type);
extern void TryToConvertType(SyntaxNode*& first, SyntaxNode*& second);
extern void TryToConvertType(SyntaxNode*& expr, const SymType* type);
extern void TryToConvertTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err);
extern void TryToConvertTypeOrDie(SyntaxNode*& expr, const SymType* type, Token tok_err);

class Parser{
private:
    SyntaxNode* syntax_tree;
    Scanner& scan;
    SynTable top_sym_table;
    SymType* top_type_bool;
    std::vector<SynTable*> sym_table_stack;
    void CheckTokOrDie(TokenValue tok_val);
    void CheckNextTokOrDie(TokenValue tok_val);    
    SyntaxNode* GetIntExprOrDie();
    SyntaxNode* ParseFactor();
    SyntaxNode* ParseAddingExpr();
    SyntaxNode* ParseMultiplyingExpr();
    SyntaxNode* ParseUnaryExpr();
    SyntaxNode* ParseRelationalExpr();
    void Error(string msg, Token err_pos_tok);
    void Error(string msg);
    SymType* ParseArrayType();
    SymType* ParseRecordType();
    SymType* ParsePointerType();
    SymType* ParseType();
    void ParseVarDefinitions(bool is_global = true);
    void ParseTypeDefinitions();
    void ParseDeclarations(bool is_global = true);
    void ParseFunctionDefinition();    
    NodeStatement* ParseBlockStatement();
    NodeStatement* ParseStatement();
    NodeStatement* ParseForStatement();
    NodeStatement* ParseWhileStatement();
    NodeStatement* ParseUntilStatement();
    NodeStatement* ParseIfStatement();
    NodeStatement* ParseAssignStatement();
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
