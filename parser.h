#ifndef PARSER
#define PARSER

#include "scanner.h"
#include <string.h>
#include <exception>

#include <ostream>

enum ExpressionClass{
    EXPRESSION,
    CONSTANT,
    VARIBLE,
    UN_OPER,
    BIN_OPER,
};

struct Expression{
    Token token;
    Expression* pred;
    Expression();
    Expression(Token token_, Expression* pred_ = NULL);
    virtual ExpressionClass GetType() { return EXPRESSION; }
};

struct BinOper: public Expression{
    Expression* left;
    Expression* right;
    BinOper(Token token_, Expression* pred_ = NULL, Expression* left_ = NULL, Expression* right_ = NULL);
    virtual ExpressionClass GetType()  { return BIN_OPER; }
};

struct UnOper: public Expression{
    Expression* child;
    UnOper(Token token_, Expression* pred_ = NULL, Expression* child_ = NULL);
    virtual ExpressionClass GetType()  { return UN_OPER; }
};

struct Varible: public Expression{
    Varible(Token token_, Expression* pred_ = NULL);
    virtual ExpressionClass GetType()  { return VARIBLE; }
};

struct Constant: public Expression{
    Constant(Token token_, Expression* pred_ = NULL);
    virtual ExpressionClass GetType()  { return CONSTANT; }
};

class Parser{
public:
private:
    Scanner& scan;
    Token token;
    void NextToken();
    Expression* GetExpression();
    Expression* GetTerm();
    Expression* GetTermToken();
    bool IsExprOp(const Token& token) const;
    bool IsTermOp(const Token& token) const;
    bool IsConst(const Token& token) const;
    void Error(char* msg);
    void PrintNode(ostream& o, Expression* e, int margin = 1);
public:
    Parser(Scanner& scanner);
    ostream& operator<<(ostream& o);
};

#endif
