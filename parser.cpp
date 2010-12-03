#include "parser.h"

SyntaxNode* ConvertType(SyntaxNode* node, const SymType* type)
{
    if (node->GetSymType() == type) return node;
    if (node->GetSymType() == top_type_int && type == top_type_real)
        return new NodeIntToRealConv(node, top_type_real);
    return NULL;
}

void TryToConvertType(SyntaxNode*& first, SyntaxNode*& second)
{
    if (first->GetSymType() == second->GetSymType()) return;
    SyntaxNode* tmp = ConvertType(first, second->GetSymType());
    if (tmp != NULL)
    {
        first = tmp;
        return;
    }
    tmp = ConvertType(second, first->GetSymType());
    if (tmp != NULL) second = tmp;
}

void TryToConvertType(SyntaxNode*& expr, const SymType* type)
{
    if (expr->GetSymType() == type) return;
    SyntaxNode* tmp = ConvertType(expr, type);
    if (tmp == NULL) return;
    expr = tmp;
    return;
}

void TryToConvertTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err)
{
    TryToConvertType(first, second);
    if (first->GetSymType() != second->GetSymType()) 
    {
        std::stringstream s;
        s << "incompatible types: ";
        first->GetSymType()->Print(s, 0);
        s << " and ";
        second->GetSymType()->Print(s, 0);
        Error(s.str(), tok_err);
    }
}

void TryToConvertTypeOrDie(SyntaxNode*& expr, const SymType* type, Token tok_err)
{
    TryToConvertType(expr, type);
    if (expr->GetSymType() != type) 
    {
        std::stringstream s;
        s << "incompatible types: ";
        expr->GetSymType()->Print(s, 0);
        s << " and ";
        type->Print(s, 0);
        Error(s.str(), tok_err);
    }
}

//---Parser---

void Parser::PrintSyntaxTree(ostream& o)
{
    if (syntax_tree != NULL) syntax_tree->Print(o, 0);
}

void Parser::PrintSymTable(ostream& o)
{
//    for (std::vector<SynTable*>::const_iterator it = sym_table_stack.begin(); it != sym_table_stack.end(); ++it)
//        (*it)->Print(o);
    sym_table_stack.back()->Print(o, 0);
}

Parser::Parser(Scanner& scanner):
    syntax_tree(NULL),
    scan(scanner)
{
    scan.NextToken();
    top_sym_table.Add(top_type_int);
    top_sym_table.Add(top_type_real);
    sym_table_stack.push_back(&top_sym_table);
    sym_table_stack.push_back(new SynTable());
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
            const Symbol* res = FindSymbol(scan.GetToken());
            if (res == NULL) Error("identifier not found");
            if (!(res->GetClassName() && SYM_TYPE)) Error("type identifier expected");
            scan.NextToken();
            return (SymType*)res;
        }
    }
}

void Parser::ParseVarDefinitions(bool is_global)
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
            if (is_global)
                sym_table_stack.back()->Add(new SymVarGlobal(*it, type));
            else
                sym_table_stack.back()->Add(new SymVarLocal(*it, type));
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

void Parser::ParseDeclarations(bool is_global)
{
    bool loop = true;
    while (loop)
    {
        switch (scan.GetToken().GetValue()) {
            case TOK_VAR:
                scan.NextToken();
                ParseVarDefinitions(is_global);
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

void Parser::ParseFunctionDefinition()
{
    SymProc* res = NULL;
    TokenValue op = scan.GetToken().GetValue();
    if (op != TOK_PROCEDURE && op != TOK_FUNCTION) return;
    Token name = scan.NextToken();
    if (FindSymbol(name) != NULL) Error("duplicate identifier");
    if (op == TOK_PROCEDURE)
        res = new SymProc(name);
    else
        res = new SymFunct(name);
    sym_table_stack.back()->Add(res);
    sym_table_stack.push_back(new SynTable);
    res->AddSymTable(sym_table_stack.back());
    scan.NextToken();
    if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT)
    {
        scan.NextToken();
        if (scan.GetToken().GetValue() == TOK_BRACKETS_RIGHT) Error("empty formal parameters list");
        bool was_semicolon = true;
        while (was_semicolon || (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT))
        {
            bool by_ref = false;
            if (by_ref = (scan.GetToken().GetValue() == TOK_VAR)) scan.NextToken();
            vector<Token> v;
            bool was_comma = true;
            while (was_comma || (scan.GetToken().GetValue() != TOK_COLON))
            {
                if (!scan.GetToken().IsVar()) Error("identifier expected");
                v.push_back(scan.GetToken());
                if (was_comma = (scan.NextToken().GetValue() == TOK_COMMA))
                    scan.NextToken();
            }
            if (scan.GetToken().GetValue() != TOK_COLON) Error("':' expected");
            scan.NextToken();
            const SymType* type = (SymType*)FindSymbolOrDie(scan.GetToken(), SYM_TYPE, "type identifier expected");
            for (vector<Token>::iterator it = v.begin(); it != v.end(); ++it)
            {
                SymVarParam* param = new SymVarParam(*it, type, by_ref);
                sym_table_stack.back()->Add(param);
                res->AddParam(param);
            }
            if (was_semicolon = (scan.NextToken().GetValue() == TOK_SEMICOLON)) scan.NextToken();
        }
        if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT) Error("')' expected");
        scan.NextToken();
    }
    if (op == TOK_FUNCTION)
    {
        if (scan.GetToken().GetValue() != TOK_COLON) Error("':' expected");
        const SymType* type = (SymType*)FindSymbolOrDie(scan.NextToken(), SYM_TYPE, "type identifier expected");
        ((SymFunct*)res)->AddResultType(type);
        sym_table_stack.back()->Add(new SymVar(Token("Result", IDENTIFIER, TOK_UNRESERVED, -1, -1), type));
        scan.NextToken();             
    }
    if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
    scan.NextToken();
    ParseDeclarations(false);
    res->AddBody(ParseStatement());
    if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
    sym_table_stack.pop_back();
}

NodeStatement* Parser::ParseStatement()
{
    switch (scan.GetToken().GetValue())
    {
        case TOK_BEGIN:
            return ParseBlockStatement();
        case TOK_FOR:
            return ParseForStatement();
        case TOK_WHILE:
            return ParseWhileStatement();
        case TOK_REPEAT:
            return ParseUntilStatement();
        case TOK_IF:
            return ParseIfStatement();
        default:
            return ParseAssignStatement();
    }
}

NodeStatement* Parser::ParseBlockStatement()
{
    StmtBlock* block = new StmtBlock();
    scan.NextToken();
    while (scan.GetToken().GetValue() != TOK_END)
    {
        block->AddStatement(ParseStatement());
        if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
        scan.NextToken();
    }
    scan.NextToken();
    return block;
}

NodeStatement* Parser::ParseForStatement()
{
    if (!scan.NextToken().IsVar()) Error("identifier expected");
    const SymVar* index = (SymVar*)FindSymbolOrDie(scan.GetToken(), SYM_VAR, "identifier not found");
    if (index->GetVarType() != top_type_int) Error("integer varible expected");
    if (scan.NextToken().GetValue() != TOK_ASSIGN) Error("':=' expected");
    scan.NextToken();
    SyntaxNode* first = GetRelationalExpr();
    if (first == NULL) Error("expression expected");
    TryToConvertTypeOrDie(first, top_type_int, scan.GetToken());
    bool is_inc = (scan.GetToken().GetValue() == TOK_TO);
    if (!is_inc && scan.GetToken().GetValue() != TOK_DOWNTO) Error("'to' or 'downto' expected");
    scan.NextToken();
    SyntaxNode* second = GetRelationalExpr();
    if (second == NULL) Error("expression expected");
    TryToConvertTypeOrDie(second, top_type_int, scan.GetToken());
    if (scan.GetToken().GetValue() != TOK_DO) Error("'do' expected");
    scan.NextToken();
    NodeStatement* body = ParseStatement();
    if (body = NULL) Error("statement expected");
    return new StmtFor(index, first, second, is_inc, body);
}

NodeStatement* Parser::ParseWhileStatement()
{
    scan.NextToken();
    SyntaxNode* cond = GetRelationalExpr();
    if (cond == NULL) Error("expression expected");
    if (cond->GetSymType() != top_type_int) Error("integer expression expected");
    if (scan.GetToken().GetValue() != TOK_DO) Error("'do' expected");
    scan.NextToken();
    NodeStatement* body = ParseStatement();
    if (body == NULL) Error("statement expected");
    return new StmtWhile(cond, body); 
}

NodeStatement* Parser::ParseUntilStatement()
{
    StmtBlock* body = new StmtBlock();
    scan.NextToken();
    while (scan.GetToken().GetValue() != TOK_UNTIL)
    {
        body->AddStatement(ParseStatement());
        if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
        scan.NextToken();
    }
    scan.NextToken();
    SyntaxNode* cond = GetRelationalExpr();
    if (cond == NULL) Error("expression expected");
    if (cond->GetSymType() != top_type_int) Error("integer exppression expected");
    return new StmtUntil(cond, body);
}

NodeStatement* Parser::ParseIfStatement()
{
    scan.NextToken();
    SyntaxNode* cond = GetRelationalExpr();
    if (cond == NULL) Error("expression expected");
    if (cond->GetSymType() != top_type_int) Error("integer exppression expected");
    if (scan.GetToken().GetValue() != TOK_THEN) Error("'then' expected");
    scan.NextToken();
    NodeStatement* then_branch = ParseStatement();
    NodeStatement* else_branch = NULL;
    if (then_branch == NULL) Error("statement expected");
    if (scan.GetToken().GetValue() == TOK_ELSE)
    {
        scan.NextToken();
        else_branch = ParseStatement();
        if (else_branch == NULL) Error("statement expected");
    }
    if (scan.GetToken().GetValue() != TOK_SEMICOLON) Error("';' expected");
    return new StmtIf(cond, then_branch, else_branch);
}

NodeStatement* Parser::ParseAssignStatement()
{
    SyntaxNode* left = GetRelationalExpr();
    if (left == NULL) return NULL;
    if (scan.GetToken().GetValue() != TOK_ASSIGN)
        return new StmtExpression(left);
    Token op = scan.GetToken();
    scan.NextToken();
    SyntaxNode* right = GetRelationalExpr();
    if (right == NULL) Error("expression expected");
    TryToConvertTypeOrDie(right, left->GetSymType(),  op);
    return new StmtAssign(op, left, right);    
}

const Symbol* Parser::FindSymbol(Symbol* sym)
{
    const Symbol* res = NULL;
    for (std::vector<SynTable*>::const_reverse_iterator it = sym_table_stack.rbegin();
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
            case TOK_PROCEDURE:
            case TOK_FUNCTION:
                ParseFunctionDefinition();
            break;
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
                syntax_tree = ParseStatement();
                if (scan.GetToken().GetValue() != TOK_DOT) Error("'.' expected");
                loop = false;
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
        return new NodeVar(new SymVarConst(token, type));
    }
    if (left == NULL)
    {
        if (scan.GetToken().GetType() != IDENTIFIER) return NULL;
        const Symbol* sym = FindSymbolOrDie(scan.GetToken(), SymbolClass(SYM_VAR | SYM_PROC), "identifier not found");
        if (sym->GetClassName() & SYM_VAR)
        {
            left = new NodeVar((SymVar*)sym);
            scan.NextToken();
        }
        else if (sym->GetClassName() & SYM_PROC)
        {
            NodeCall* funct = new NodeCall((SymProc*)sym);
            if (scan.NextToken().GetValue() == TOK_BRACKETS_LEFT)
            {
                scan.NextToken();
                while (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                {
                    SyntaxNode* arg = GetRelationalExpr();
                    if (arg == NULL) Error("illegal expression");
                    if (scan.GetToken().GetValue() == TOK_COMMA)
                        scan.NextToken();
                    else if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                        Error(", expected");
                    if (funct->GetCurrentArgType() == NULL) Error("too many actual parameters");
                    TryToConvertTypeOrDie(arg, funct->GetCurrentArgType(), scan.GetToken());
                    if (funct->IsCurrentArfByRef() && !arg->IsLValue()) Error("lvalue expected");
                    funct->AddArg(arg);
                }
                scan.NextToken();
                left = funct;
            }
            if (funct->GetCurrentArgType() != NULL) Error("not enough actual parameters");
            left = funct;
        }
        else Error("identifier expected");
    }
    Token op = scan.GetToken();
    while (op.IsTermOp())
    {
        if (op.GetValue() == TOK_DOT)
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
        TryToConvertTypeOrDie(left, right, op);
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
        TryToConvertTypeOrDie(left, right, op);
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
        TryToConvertTypeOrDie(left, right, op);
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
