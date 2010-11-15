#include "parser.h"

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

static void PrintSpaces(ostream& o, int count)
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

void Parser::PrintSimpleParse(ostream& o)
{
    Expression* root = GetRelationalExpr();
    if (scan.GetToken().GetType() != END_OF_FILE)
        Error("end of file expected");
    PrintNode(o, root);
}

void Parser::PrintDeclarationsParse(ostream& o)
{
//    ParseVarDefinitions();
    Parse();
    syn_table.Print(o);
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
    syn_table.Add(new SymTypeInteger(Token("Integer", RESERVED_WORD, TOK_INTEGER, -1, -1)));
    syn_table.Add(new SymTypeReal(Token("Real", RESERVED_WORD, TOK_REAL, -1, -1)));
//debug    syn_table.Print(std::cout);
    syn_table_stack.push_back(&syn_table);
}

SymType* Parser::ParseType()
{
    Token name = scan.GetToken();
    switch (name.GetValue()) {
        case TOK_ARRAY: {
            std::vector<std::pair<Token, Token> > bounds;
            if (scan.NextToken().GetValue() != TOK_BRACKETS_SQUARE_LEFT) Error("'[' expected");
            bool was_comma = true;
            while (was_comma)
            {
                Token low = scan.NextToken();
                if (scan.NextToken().GetValue() != TOK_DOUBLE_DOT) Error("'..' expected");
                Token high = scan.NextToken();
                if (low.GetType() != INT_CONST || high.GetType() != INT_CONST)
		    Error("non-integer array bounds is not implemented");
                bounds.push_back(std::pair<Token, Token>(low, high));
                was_comma = (scan.NextToken().GetValue() == TOK_COMMA);
            }
            if (scan.GetToken().GetValue() != TOK_BRACKETS_SQUARE_RIGHT) Error("']' expected");
            if (scan.NextToken().GetValue() != TOK_OF) Error("'of' expected");
            scan.NextToken();
            SymType* res = ParseType();
            for (std::vector<std::pair<Token, Token> >::reverse_iterator it = bounds.rbegin(); it != bounds.rend(); ++it)
            {
                res = new SymTypeArray(name, res, atoi(it->first.GetName()), atoi(it->second.GetName()));
            }
            return res;
        } break;
        case TOK_RECORD: {
            syn_table_stack.push_back(new SynTable);
            scan.NextToken();
            ParseVarDefinitions();
            SymType* res = new SymTypeRecord(name, syn_table_stack.back());
            syn_table_stack.pop_back();
            if (scan.GetToken().GetValue() != TOK_END) Error("'end' expected");
            scan.NextToken();
            return res;
        } break;
        case TOK_CAP: {
            Token name = scan.NextToken();
            if (!name.IsVar()) Error("identifier expected");
            const Symbol* ref_type = syn_table_stack.back()->Find(name);
            if (ref_type == NULL) Error("identifier not found");
            if (!(ref_type->GetClassName() && SYM_TYPE)) Error("type identifier expected");
            scan.NextToken();
            return new SymTypeAlias(name, (SymType*)ref_type);
        } break;
        default: {            
            const Symbol* res = syn_table_stack.back()->Find(scan.GetToken());
//debug            syn_table_stack.back()->Print(std::cout);
            if (res == NULL) Error("identifier not found");
            if (!(res->GetClassName() && SYM_TYPE)) Error("type identifier expected");
            scan.NextToken();
            return (SymType*)res;
        }
    }
}

void Parser::ParseVarDefinitions()
{
    while (scan.GetToken().IsVar())
    {
        std::vector<Token> vars;
        bool was_comma = true;
        while (was_comma)
        {
            Token name = scan.GetToken();
            if (FindSymbol(name) != NULL) Error("duplicate declaration");
            vars.push_back(name);
            if (was_comma = (scan.NextToken().GetValue() == TOK_COMMA))
                scan.NextToken();
        }

        if (scan.GetToken().GetValue() != TOK_COLON) Error("':' expected");
        scan.NextToken();
        SymType* type = ParseType();
        for (std::vector<Token>::iterator it = vars.begin(); it != vars.end(); ++it)
	    syn_table_stack.back()->Add(new SymVar(*it, type));
        if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
        scan.NextToken();
    }
}

void Parser::ParseTypeDefinitions()
{
    while (scan.GetToken().IsVar())
    {
        Token name = scan.GetToken();
        if (FindSymbol(name) != NULL) Error("duplicate declaration");
        if (scan.GetToken().GetValue() != TOK_EQUAL) Error("'=' expected");
        scan.NextToken();
        SymType* type = ParseType();
        syn_table_stack.back()->Add(new SymTypeAlias(name, type));
        if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
        scan.NextToken();
    }
}

void Parser::ParseStatement()
{
}

#include <iostream>

const Symbol* Parser::FindSymbol(Symbol* sym)
{
    const Symbol* res = NULL;
    for (std::vector<SynTable*>::reverse_iterator it = syn_table_stack.rbegin();
         it != syn_table_stack.rend() && res == NULL; ++it)
    {
        res = (*it)->Find(sym);
        if (res != NULL)
        {
            (*it)->Print(std::cout);
        }
    }
    return res;
}

const Symbol* Parser::FindSymbol(const Token& tok)
{
    Symbol sym(tok);
    return FindSymbol(&sym);
}

void Parser::Parse()
{
    bool loop = true;
    while (loop)
    {
        switch (scan.GetToken().GetValue()) {
            case TOK_VAR:
                scan.NextToken();
                ParseVarDefinitions();
            break;
            case TOK_TYPE:
                scan.NextToken();
                ParseTypeDefinitions();
            break;
            default:
                loop = false;
        }      
    }
}

Expression* Parser::GetTerm()
{
    Expression* left = NULL;
    if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT)
    {
        scan.NextToken();
        left = GetRelationalExpr();
        if (left == NULL) Error("illegal expression");
        if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
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
        if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT)
        {
            scan.NextToken();
            FunctionCall* funct = new FunctionCall(op, left);
            while (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
            {
                Expression* arg = GetRelationalExpr();
                if (arg == NULL) Error("illegal expression");
                if (scan.GetToken().GetValue() == TOK_COMMA)
                    scan.NextToken();
                else if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                    Error(", expected");
                funct->AddArgument(arg);
            }
            scan.NextToken();
            left = funct;
        }
        else if (op.GetValue() == TOK_DOT)
        {
            while (op.GetValue() == TOK_DOT)
            {
                Token field = scan.NextToken();
                if (scan.GetToken().GetType() != IDENTIFIER) Error("identifier after . expected");
                left = new RecordAccess(op, field, left);
                op = scan.NextToken();
            }
        }
        else if (op.GetValue() == TOK_BRACKETS_SQUARE_LEFT)
        {
            scan.NextToken();
            do {
                Expression* index = GetRelationalExpr();
                if (index == NULL) Error("illegal expression");
                if (scan.GetToken().GetValue() == TOK_COMMA)
                    scan.NextToken();
                else if (scan.GetToken().GetValue() != TOK_BRACKETS_SQUARE_RIGHT)
                    Error(", expected");
                left = new ArrayAccess(op, left, index);
            } while (scan.GetToken().GetValue() != TOK_BRACKETS_SQUARE_RIGHT);
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
