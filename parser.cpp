#include "parser.h"

static void PrintSpaces(ostream& o, int count)
{
    for (int i = 0; i < count; ++i) o << ' ';
}

//---Parser---

void Parser::PrintSyntaxTree(ostream& o)
{
    if (syntax_tree != NULL) syntax_tree->Print(o, 0);
}

void Parser::PrintSymTable(ostream& o)
{
    for (std::vector<SynTable*>::const_iterator it = sym_table_stack.begin(); it != sym_table_stack.end(); ++it)
        (*it)->Print(o);
}

Parser::Parser(Scanner& scanner):
    syntax_tree(NULL),
    scan(scanner)
{
    scan.NextToken();
    top_sym_table.Add(top_type_int);
    top_sym_table.Add(top_type_real);
    sym_table_stack.push_back(&top_sym_table);
    Parse();
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
            sym_table_stack.push_back(new SynTable);
            scan.NextToken();
            ParseVarDefinitions();
            SymType* res = new SymTypeRecord(name, sym_table_stack.back());
            sym_table_stack.pop_back();
            if (scan.GetToken().GetValue() != TOK_END) Error("'end' expected");
            scan.NextToken();
            return res;
        } break;
        case TOK_CAP: {
            Token name = scan.NextToken();
            if (!name.IsVar()) Error("identifier expected");
            const Symbol* ref_type = sym_table_stack.back()->Find(name);
            if (ref_type == NULL) Error("identifier not found");
            if (!(ref_type->GetClassName() && SYM_TYPE)) Error("type identifier expected");
            scan.NextToken();
            return new SymTypePointer(name, (SymType*)ref_type);
        } break;
        default: {            
            //const Symbol* res = sym_table_stack.back()->Find(scan.GetToken());
            const Symbol* res = FindSymbol(scan.GetToken());
//debug            sym_table_stack.back()->Print(std::cout);
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
	    sym_table_stack.back()->Add(new SymVar(*it, type));
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
        if (scan.NextToken().GetValue() != TOK_EQUAL) Error("'=' expected");
        scan.NextToken();
        SymType* type = ParseType();
        sym_table_stack.back()->Add(new SymTypeAlias(name, type));
        if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
        scan.NextToken();
    }
}

const Symbol* Parser::FindSymbol(Symbol* sym)
{
    const Symbol* res = NULL;
    for (std::vector<SynTable*>::reverse_iterator it = sym_table_stack.rbegin();
         it != sym_table_stack.rend() && res == NULL; ++it)
    {
        res = (*it)->Find(sym);
    }
    return res;
}

const Symbol* Parser::FindSymbolOrDie(Symbol* sym, SymbolClass type, string msg)
{
    const Symbol* res = FindSymbol(sym);
    if (res == NULL) Error("identifier not found");
    if (!(res->GetClassName() & type)) Error(msg);
    return res;
}

const Symbol* Parser::FindSymbolOrDie(Token tok, SymbolClass type, string msg)
{
    Symbol sym(tok);
    return FindSymbolOrDie(&sym, type, msg);
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
            case TOK_BEGIN:
            {
                scan.NextToken();
                SyntaxNode* left = GetRelationalExpr();
                SyntaxNode* right = NULL;
                if (left != NULL && scan.GetToken().GetValue() == TOK_ASSIGN)
                {
                    Token op = scan.GetToken();
                    scan.NextToken();
                    right = GetRelationalExpr();
                    if (right == NULL) Error("expression expected");
                    left = new StmtAssign(op, left, right);
                }
                if (left != NULL)
                {
                    if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
                    scan.NextToken();
                }
                if (scan.GetToken().GetValue() != TOK_END) Error("'end' expected");
                if (scan.NextToken().GetValue() != TOK_DOT) Error("'.' expected");
                if (scan.NextToken().GetType() != END_OF_FILE) Error("end of file expected");
                loop = false;
                syntax_tree = left;
            }
            default:
                loop = false;
        }      
    }
}

SyntaxNode* Parser::GetTerm()
{
    SyntaxNode* left = NULL;
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
        SymType* type = NULL;
        if (token.GetType() == INT_CONST) type = top_type_int;
        else if (token.GetType() == REAL_CONST) type = top_type_real;
        else Error("operations on string const not implemented");
        scan.NextToken();
        return new NodeVar(new SymVarParam(token, type));
    }
    if (left == NULL)
    {
        if (scan.GetToken().GetType() != IDENTIFIER) return NULL;
        const Symbol* sym = FindSymbolOrDie(scan.GetToken(), SYM_VAR, "varible expected");
        scan.NextToken();
        left = new NodeVar((SymVar*)sym);
    }
    Token op = scan.GetToken();
    while (op.IsTermOp())
    {
        if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT)
        {
            NodeCall* funct = new NodeCall(left);
            scan.NextToken();
            while (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
            {
                SyntaxNode* arg = GetRelationalExpr();
                if (arg == NULL) Error("illegal expression");
                if (scan.GetToken().GetValue() == TOK_COMMA)
                    scan.NextToken();
                else if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                    Error(", expected");
                funct->AddArg(arg);
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
                left = new NodeRecordAccess((NodeVar*)left, field);
                op = scan.NextToken();
            }
        }
        else if (op.GetValue() == TOK_BRACKETS_SQUARE_LEFT)
        {
            scan.NextToken();
            while (true) {
                SyntaxNode* index = GetRelationalExpr();
                if (index == NULL) Error("illegal expression");
                left = new NodeArrayAccess(left, index, op);
                op = scan.GetToken();
                if (op.GetValue() == TOK_COMMA) scan.NextToken();
                else if (op.GetValue() == TOK_BRACKETS_SQUARE_RIGHT)
                {
                    if (scan.NextToken().GetValue() == TOK_BRACKETS_SQUARE_LEFT)
                        scan.NextToken();
                    else
                        break;
                }
                else Error("illegal expression");
            }
        }
        op = scan.GetToken();
    }
    return left;
}

SyntaxNode* Parser::GetUnaryExpr()
{
    std::vector<Token> un;
    while (scan.GetToken().IsUnaryOp())
    {
        un.push_back(scan.GetToken());
        scan.NextToken();
    }
    SyntaxNode* res = GetTerm();
    if (un.size() != 0 && res == NULL) Error("illegal expression");
    if (res != NULL)
    {
        for (std::vector<Token>::reverse_iterator it = un.rbegin(); it != un.rend(); ++it)
            res = new NodeUnaryOp(*it, res);
    }
    return res;
}

SyntaxNode* Parser::GetMultiplyingExpr()
{
    SyntaxNode* left = GetUnaryExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsMultOp())
    {
        scan.NextToken();
        SyntaxNode* right = GetUnaryExpr();
        if (right == NULL) Error("illegal expression");
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

SyntaxNode* Parser::GetAddingExpr()
{
    SyntaxNode* left = GetMultiplyingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsAddingOp())
    {
        scan.NextToken();
        SyntaxNode* right = GetMultiplyingExpr();
        if (right == NULL) Error("expression expected");
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

SyntaxNode* Parser::GetRelationalExpr()
{
    SyntaxNode* left = GetAddingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsRelationalOp())
    {
        scan.NextToken();
        SyntaxNode* right = GetAddingExpr();
        if (right == NULL) Error("expression expected");
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

void Parser::Error(string msg)
{
    stringstream s;
    Token token = scan.GetToken();
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}
