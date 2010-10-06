#include "parser.h"


#include <vector>

vector<bool> margins;

void Parser::PrintNode(ostream& o, Expression* e, int margin)
{
    if (e == NULL) return;
    //for (int i = 0; i < margin - 1; ++i) o<< ": ";
    //if (margin) o << ":.";
    if (margins.size())
    {
        bool tmp = margins.back();
        margins.pop_back();
        for (vector<bool>::iterator it = margins.begin(); it != margins.end(); ++it)
            if (*it)
                o << "| ";
            else
                o << "  ";
        margins.push_back(tmp);
    }
    if (margins.size()) o << "|-";
    if (e -> token.GetType() == OPERATION)
        o << '\'' <<  e -> token.GetValue() << '\'' << "\n"  ;
    else
        o << e -> token.GetValue() << "\n"  ;
    switch (e -> GetType())
    {
        case CONSTANT:
        break;
        case VARIBLE:
        break;
        case UN_OPER:
            PrintNode(o, ((UnOper*)e) -> child, 0);
        break;
        case BIN_OPER:
            margins.push_back(true);
            PrintNode(o, ((BinOper*)e) -> left, 1);
            --margin;
            if (!margin)
            {
                margins.pop_back();
                margins.push_back(false);
            }
            PrintNode(o, ((BinOper*)e) -> right, 1);
            margins.pop_back();
        break;
    }

}

/*
void Parser::PrintNode(ostream& o, Expression* e, int margin)
{
    if (e == NULL) return;
    for (int i = 0; i < margin; ++i) o<< ' ';
    o <<  e -> token.GetValue() << "\n"  ;
    switch (e -> GetType())
    {
        case CONSTANT:
        break;
        case VARIBLE:
        break;
        case UN_OPER:
            PrintNode(o, ((UnOper*)e) -> child, margin + 2);
        break;
        case BIN_OPER:
            PrintNode(o, ((BinOper*)e) -> left, margin + 2);
            PrintNode(o, ((BinOper*)e) -> right, margin + 2);
        break;
    }

}
*/

ostream& Parser::operator<<(ostream& o)
{
    Expression* root = GetExpression();
    PrintNode(o, root);
}


//---Expression---

Expression::Expression():
    pred(NULL)
{
}

Expression::Expression(Token token_, Expression* pred_):
    token(token_),
    pred(pred_)
{
}

//---BinOper---

BinOper::BinOper(Token token_, Expression* pred_, Expression* left_, Expression* right_):
    Expression(token_, pred_),
    left(left_),
    right(right_)
{
}

//---UnOper---

UnOper::UnOper(Token token_, Expression* pred_, Expression* child_):
    Expression(token_, pred_),
    child(child_)
{
}

//---Varible---

Varible::Varible(Token token_, Expression* pred_):
    Expression(token_, pred_)
{
}

//---Constant---

Constant::Constant(Token token_, Expression* pred_):
    Expression(token_, pred_)
{
}

//---Expression---

void Parser::NextToken()
{
    token = scan.NextToken();
}

Parser::Parser(Scanner& scanner):
    scan(scanner)
{
    scan.NextToken();
}

Expression* Parser::GetExpression()
{
    Expression* left = GetTerm();
    Token op = scan.GetToken();
    while (IsExprOp(op))
    {
        scan.NextToken();
        Expression* right = GetTerm();
        if (right == NULL) Error("expression expected");
        left = new BinOper(op, NULL, left, right);
        op = scan.GetToken();
    }
    return left;
}

bool Parser::IsConst(const Token& token) const
{
    TokenType type = token.GetType();
    return type == HEX_CONST || type == INT_CONST || type == REAL_CONST;
}

Expression* Parser::GetTermToken()
{
    Expression* res = NULL;
    Token token = scan.GetToken();
    if (token.GetType() == IDENTIFIER)
    {
        res = new Varible(token);
        scan.NextToken();
    }
    else if (IsConst(token))
    {
        res = new Constant(token);
        scan.NextToken();
    }
    else
    {
        scan.NextToken();
        res = GetExpression();
        token = scan.GetToken();
        if (token.GetType() != DELIMITER || strcmp(token.GetValue(), ")"))
            Error("expected )");
        scan.NextToken();
    }
    return res;
}

Expression* Parser::GetTerm()
{
    Expression* left = NULL;
    UnOper* root = NULL;
    UnOper* first_root = NULL;
    Token token = scan.GetToken();
    while (IsExprOp(token))
    {
        root = new UnOper(token, NULL, root);
        if (first_root == NULL) first_root = root;
        token = scan.NextToken();
    }
    left = GetTermToken();
    token = scan.GetToken();
    while (IsTermOp(token))
    {
        if (!IsTermOp(token))
            break;
        NextToken();
        Expression* right = GetTermToken();
        left = new BinOper(token, NULL, left, right);
        token = scan.GetToken();
    }
    if (root != NULL)
    {
        left -> pred = first_root;
        first_root -> child = left;
        return root;
    }
    return left;
}

bool Parser::IsExprOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "+") || !strcmp(token.GetValue(), "-");
}

bool Parser::IsTermOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "*") || !strcmp(token.GetValue(), "/") ||
           !strcmp(token.GetValue(), "div") || !strcmp(token.GetValue(), "mod");
}

void Parser::Error(char* msg)
{

}
