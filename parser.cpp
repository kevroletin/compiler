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
        o << '\'' <<  e -> token.GetName() << '\'' << "\n"  ;
    else
        o << e -> token.GetName() << "\n"  ;
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
            o << e -> token.GetName() << "\n";
        break;
        case VARIBLE:
            o << e -> token.GetName() << "\n";
        break;
        case UN_OPER:
            o << e -> token.GetName() << "\n";
            PrintNode(o, ((UnOper*)e) -> child, margin + 2);
        break;
        case BIN_OPER:
            o << e -> token.GetName() << "\n";
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
            o << ((RecordAccess*)e) -> field.GetName() << "\n";
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

SymType* Parser::ParseType()
{
    Token name = scan.GetToken();
    if (!name.IsVar()) return NULL;
    scan.NextToken();
    //if (scan.NextToken() != tSemicolon)
    if (strcmp(scan.NextToken().GetName(), ":")) Error("':' expected"); //плохо
    Token token = scan.NextToken();

    scan.NextToken();
}

void Parser::ParseDeclarations()
{
    Symbol* sym = NULL;
   /*
    while (sym = ParseType()) {
        if (syn_table.Find(sym)) Error("duplicate declaration");
        syn_table.Add(sym);
    }
    */
}

Expression* Parser::GetTerm()
{
    Expression* left = NULL;
    if (!strcmp(scan.GetToken().GetName(), "("))
    {
        scan.NextToken();
        left = GetRelationalExpr();
        if (left == NULL) Error("illegal expression");
        if (strcmp(scan.GetToken().GetName(), ")"))
            Error("expected )");
        scan.NextToken();
    }
    if (scan.GetToken().IsConst())
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
    while (op.IsTermOp())
    {
        if (!strcmp(op.GetName(), "("))
        {
            scan.NextToken();
            FunctionCall* funct = new FunctionCall(op, left);
            while (strcmp(scan.GetToken().GetName(), ")"))
            {
                Expression* arg = GetRelationalExpr();
                if (arg == NULL) Error("illegal expression");
                if (!strcmp(scan.GetToken().GetName(), ","))
                    scan.NextToken();
                else if (strcmp(scan.GetToken().GetName(), ")"))
                    Error(", expected");
                funct->AddArgument(arg);
            }
            scan.NextToken();
            left = funct;
        }
        else if (!strcmp(op.GetName(), "."))
        {
            while (!strcmp(op.GetName(), "."))
            {
                Token field = scan.NextToken();
                if (scan.GetToken().GetType() != IDENTIFIER) Error("identifier after . expected");
                left = new RecordAccess(op, field, left);
                op = scan.NextToken();
            }
        }
        else if (!strcmp(op.GetName(), "["))
        {
            scan.NextToken();
            do {
                Expression* index = GetRelationalExpr();
                if (index == NULL) Error("illegal expression");
                if (!strcmp(scan.GetToken().GetName(), ","))
                    scan.NextToken();
                else if (strcmp(scan.GetToken().GetName(), "]"))
                    Error(", expected");
                left = new ArrayAccess(op, left, index);
            } while (strcmp(scan.GetToken().GetName(), "]"));
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
    while (token.IsUnaryOp())
    {
        UnOper* newOp = new UnOper(token, firstun);
        if (firstun != NULL) firstun -> child = newOp;
        firstun = newOp;
        if (un == NULL) un = firstun;
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
    left = GetUnaryExpr();
    if (left == NULL) return NULL;
    Token token = scan.GetToken();
    while (token.IsMultOp())
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
    while (op.IsAddingOp())
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
    while (op.IsRelationalOp())
    {
        scan.NextToken();
        Expression* right = GetAddingExpr();
        if (right == NULL) Error("expression expected");
        left = new BinOper(op, NULL, left, right);
        op = scan.GetToken();
    }
    return left;
}

void Parser::Error(char* msg)
{
    stringstream s;
    Token token = scan.GetToken();
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException( s.str().c_str() ) ) ;
}
