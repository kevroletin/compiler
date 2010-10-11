#include "parser.h"


#include <vector>

/*
vector<bool> margins;
void Parser::PrintNode(ostream& o, Expression* e, int margin = 1)
{
    if (e == NULL) return;
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
            margins.pop_back();
            margins.push_back( margin == 1);
            margins.push_back(false);
            PrintNode(o, ((UnOper*)e) -> child, 0);
            margins.pop_back();
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
            PrintNode(o, ((BinOper*)e) -> right, 0);
            margins.pop_back();
        break;
    }
}
*/

void PrintSpaces(ostream& o, int count)
{
    for (int i = 0; i < count; ++i) o << ' ';
}

void Parser::PrintNode(ostream& o, Expression* e, int margin)
{
    if (e == NULL) return;
    PrintSpaces(o, margin);
    switch (e -> GetType())
    {
        case CONSTANT:
            o << e -> token.GetValue() << "\n";
        break;
        case VARIBLE:
            o << e -> token.GetValue() << "\n";
        break;
        case UN_OPER:
            o << e -> token.GetValue() << "\n";
            PrintNode(o, ((UnOper*)e) -> child, margin + 2);
        break;
        case BIN_OPER:
            o << e -> token.GetValue() << "\n";
            PrintNode(o, ((BinOper*)e) -> left, margin + 2);
            PrintNode(o, ((BinOper*)e) -> right, margin + 2);
        break;
        case FUNCTION_CALL:
        {
            o << "()" << "\n";
            FunctionCall* f = (FunctionCall*)e;
            PrintNode(o, f->funct, margin + 2);
            for(vector<Expression*>::iterator it = f->args.begin(); it != f->args.end(); ++it)
                PrintNode(o, *it, margin + 2);
        }
        break;
        case RECORD_ACCESS:
            o << ".\n";
            PrintNode(o, ((RecordAccess*)e) -> record, margin + 2);
            PrintSpaces(o, margin + 2);
            o << ((RecordAccess*)e) -> field.GetValue() << "\n";
        break;
        case ARRAY_ACCESS:
            o << "[]\n";
            PrintNode(o, ((ArrayAccess*)e) -> arr, margin + 2);
            PrintNode(o, ((ArrayAccess*)e) -> index, margin + 2);
    }

}

ostream& Parser::operator<<(ostream& o)
{
    Expression* root = GetRelationalExpr();
    if (scan.GetToken().GetType() != END_OF_FILE)
        Error("end of file expected");
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

//---RecordAccess

RecordAccess::RecordAccess(Token token, Token field_, Expression* record_, Expression* pred_):
    Expression(token, pred_),
    field(field_),
    record(record_)
{
}

//---ArrayAccess

ArrayAccess::ArrayAccess(Token token, Expression* arr_, Expression* index_, Expression* pred_):
    Expression(token, pred_),
    arr(arr_),
    index(index_)
{
    index -> pred = this;
}

//---FunctionCall---

FunctionCall::FunctionCall(Token token, Expression* funct_, Expression* pred_):
    Expression(token, pred_),
    funct(funct_)
{
}

void FunctionCall::AddArgument(Expression* arg)
{
    args.push_back(arg);
}

//---Parser---

Parser::Parser(Scanner& scanner):
    scan(scanner)
{
    scan.NextToken();
}

Expression* Parser::GetTerm()
{
    Expression* left = NULL;
    if (!strcmp(scan.GetToken().GetValue(), "("))
    {
        scan.NextToken();
        left = GetRelationalExpr();
        if (left == NULL) Error("illegal expression");
        if (strcmp(scan.GetToken().GetValue(), ")"))
            Error("expected )");
        scan.NextToken();
    }
    if (IsConst(scan.GetToken()))
    {
        Token token = scan.GetToken();
        scan.NextToken();
        return new Constant(token);
    }
    if (left == NULL)
    {
        if (scan.GetToken().GetType() != IDENTIFIER) return NULL;
        left = new Varible(scan.GetToken());
        scan.NextToken();
    }
    Token op = scan.GetToken();
    while (IsTermOp(op))
    {
        if (!strcmp(op.GetValue(), "("))
        {
            scan.NextToken();
            FunctionCall* funct = new FunctionCall(op, left);
            while (strcmp(scan.GetToken().GetValue(), ")"))
            {
                Expression* arg = GetRelationalExpr();
                if (arg == NULL) Error("illegal expression");
                if (!strcmp(scan.GetToken().GetValue(), ","))
                    scan.NextToken();
                else if (strcmp(scan.GetToken().GetValue(), ")"))
                    Error(", expected");
                funct->AddArgument(arg);
            }
            scan.NextToken();
            left = funct;
        }
        else if (!strcmp(op.GetValue(), "."))
        {
            while (!strcmp(op.GetValue(), "."))
            {
                Token field = scan.NextToken();
                if (scan.GetToken().GetType() != IDENTIFIER) Error("identifier after . expected");
                left = new RecordAccess(op, field, left);
                op = scan.NextToken();
            }
        }
        else if (!strcmp(op.GetValue(), "["))
        {
            scan.NextToken();
            do {
                Expression* index = GetRelationalExpr();
                if (index == NULL) Error("illegal expression");
                if (!strcmp(scan.GetToken().GetValue(), ","))
                    scan.NextToken();
                else if (strcmp(scan.GetToken().GetValue(), "]"))
                    Error(", expected");
                left = new ArrayAccess(op, left, index);
            } while (strcmp(scan.GetToken().GetValue(), "]"));
            scan.NextToken();
        }
        op = scan.GetToken();
    }
    return left;
}

Expression* Parser::GetUnaryExpr()
{
    Token token = scan.GetToken();
    UnOper* un = NULL;
    UnOper* firstun = NULL;
    while (IsUnaryOp(token))
    {
        un = new UnOper(token, un);
        if (firstun == NULL) firstun = un;
        token = scan.NextToken();
    }
    Expression* res = GetTerm();
    if (un != NULL && res == NULL) Error("illegal expression");
    if (res != NULL)
    {
        if (un != NULL)
        {
            firstun -> child = res;
            res -> pred = firstun;
            return un;
        }
        return res;
    }
    return NULL;
}

Expression* Parser::GetMultiplyingExpr()
{
    Expression* left = NULL;
    UnOper* root = NULL;
    UnOper* first_root = NULL;
    Token token = scan.GetToken();
    while (IsAddingOp(token))
    {
        root = new UnOper(token, NULL, root);
        if (first_root == NULL) first_root = root;
        token = scan.NextToken();
    }
    left = GetUnaryExpr();
    if (left == NULL) return NULL;
    token = scan.GetToken();
    while (IsMultOp(token))
    {
        scan.NextToken();
        Expression* right = GetUnaryExpr();
        if (right == NULL) Error("illegal expression");
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

Expression* Parser::GetAddingExpr()
{
    Expression* left = GetMultiplyingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (IsAddingOp(op))
    {
        scan.NextToken();
        Expression* right = GetMultiplyingExpr();
        if (right == NULL) Error("expression expected");
        left = new BinOper(op, NULL, left, right);
        op = scan.GetToken();
    }
    return left;
}

Expression* Parser::GetRelationalExpr()
{
    Expression* left = GetAddingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (IsRelationalOp(op))
    {
        scan.NextToken();
        Expression* right = GetAddingExpr();
        if (right == NULL) Error("expression expected");
        left = new BinOper(op, NULL, left, right);
        op = scan.GetToken();
    }
    return left;
}

bool Parser::IsRelationalOp(const Token& token) const
{
    return !strcmp(token.GetValue(), ">") || !strcmp(token.GetValue(), ">=") ||
           !strcmp(token.GetValue(), "<") || !strcmp(token.GetValue(), "<=") ||
           !strcmp(token.GetValue(), "<>");
}

bool Parser::IsAddingOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "+") || !strcmp(token.GetValue(), "-") ||
           !strcmp(token.GetValue(), "or") || !strcmp(token.GetValue(), "xor");
}

bool Parser::IsMultOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "*") || !strcmp(token.GetValue(), "/") ||
           !strcmp(token.GetValue(), "div") || !strcmp(token.GetValue(), "mod") ||
           !strcmp(token.GetValue(), "and") || !strcmp(token.GetValue(), "shl") ||
           !strcmp(token.GetValue(), "shr");
}

bool Parser::IsUnaryOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "not") || !strcmp(token.GetValue(), "@") ||
           !strcmp(token.GetValue(), "+") || !strcmp(token.GetValue(), "-");
}

bool Parser::IsTermOp(const Token& token) const
{
    return !strcmp(token.GetValue(), "[") || !strcmp(token.GetValue(), ".") || !strcmp(token.GetValue(), "(");
}

void Parser::Error(char* msg)
{
    stringstream s;
    Token token = scan.GetToken();
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetValue() << "': " << msg;
    throw( CompilerException( s.str().c_str() ) ) ;
}

bool Parser::IsConst(const Token& token) const
{
    TokenType type = token.GetType();
    return type == HEX_CONST || type == INT_CONST || type == REAL_CONST || type == STR_CONST;
}

bool Parser::IsConstVar(const Token& token) const
{
    return IsConst(token) || token.GetType() == VARIBLE;
}
