#include "scanner.h"

bool ishexnum(char c)
{
    return isdigit(c) || ('a' <= tolower(c)  && tolower(c) <= 'f');
}

const char* const TOKEN_DESCRIPTION[] =
{
	"IDENTIFIER",
	"RESERVED_WORD",
	"HEX_CONST",
	"INT_CONST",
	"REAL_CONST",
	"STR_CONST",
	"OPERATION",
	"DELIMITER",
	"END_OF_FILE"
};

//---Reserved words--

void ReservedWords::Add(char* name, TokenType type, TokenValue value)
{
    words.insert(pair<char*, TokenType>(name, type));
}

ReservedWords::ReservedWords()
{
    Add("and", OPERATION, TOK_AND);
    Add("array", RESERVED_WORD, TOK_ARRAY);
    Add("begin", RESERVED_WORD, TOK_BEGIN);
    Add("case", RESERVED_WORD, TOK_CASE);
    Add("const", RESERVED_WORD, TOK_CONST);
    Add("div", OPERATION, TOK_DIV);
    Add("do", RESERVED_WORD, TOK_DO);
    Add("downto", RESERVED_WORD, TOK_DOWNTO);
    Add("else", RESERVED_WORD, TOK_ELSE);
    Add("end", RESERVED_WORD, TOK_END);
    Add("file", RESERVED_WORD, TOK_FILE);
    Add("for", RESERVED_WORD, TOK_FILE);
    Add("function", RESERVED_WORD, TOK_FUNCTION);
    Add("if", RESERVED_WORD, TOK_IF);
    Add("in", RESERVED_WORD, TOK_IN);
    Add("mod", OPERATION, TOK_MOD);
    Add("nil", RESERVED_WORD, TOK_NIL);
    Add("not", OPERATION, TOK_NOT);
    Add("of", RESERVED_WORD, TOK_OF);
    Add("or", OPERATION, TOK_OR);
    Add("procedure", RESERVED_WORD, TOK_PROCEDURE);
    Add("record", RESERVED_WORD, TOK_RECORD);
    Add("repeat", RESERVED_WORD, TOK_REPEAT);
    Add("set", RESERVED_WORD, TOK_SET);
    Add("shl", OPERATION, TOK_SHL);
    Add("shr", OPERATION, TOK_SHR);
    Add("string", RESERVED_WORD, TOK_STRING);
    Add("then", RESERVED_WORD, TOK_THEN);
    Add("to", RESERVED_WORD, TOK_TO);
    Add("type", RESERVED_WORD, TOK_TYPE);
    Add("until", RESERVED_WORD, TOK_UNTIL);
    Add("var", RESERVED_WORD, TOK_VAR);
    Add("while", RESERVED_WORD, TOK_WHILE);
    Add("with", RESERVED_WORD, TOK_WITH);
    Add("xor", OPERATION, TOK_XOR);
    Add("..", RESERVED_WORD, TOK_DOUBLE_DOT);
    Add(":=", OPERATION, TOK_ASSIGN);
    Add("-", OPERATION, TOK_MINUS);
    Add("+", OPERATION, TOK_PLUS);
    Add("*", OPERATION, TOK_MULT);
    Add("/", OPERATION, TOK_DIVISION);
    Add("[", OPERATION, TOK_BRACKETS_SQUARE_LEFT);
    Add("]", OPERATION, TOK_BRACKETS_SQUARE_RIGHT);
    Add(";", DELIMITER, TOK_SEMICOLON);
    Add(":", DELIMITER, TOK_COLON);
    Add(",", DELIMITER, TOK_COMMA);
    Add(".", OPERATION, TOK_DOT);
    Add("^", OPERATION, TOK_CAP);
    Add("@", OPERATION, TOK_DOG);
    Add("(", OPERATION, TOK_BRACKETS_LEFT);
    Add(")", OPERATION, TOK_BRACKETS_RIGHT);
    Add(">", OPERATION, TOK_BRACKETS_ANGLE_RIGHT);
    Add("<", OPERATION, TOK_BRACKETS_ANGLE_LEFT);
    Add("=", OPERATION, TOK_EQUAL);
    Add(">=", OPERATION, TOK_GREATER_OR_EQUAL);
    Add("<=", OPERATION, TOK_LESS_OR_EQUAL);
    Add("<>", OPERATION, TOK_NOT_EQUAL);
}

bool ReservedWords::Identify(string& str, TokenType& returned_type)
{
    map<string, TokenType>::iterator i  = words.find(str);
    if (i == words.end()) return false;
    returned_type = i->second;
    return true;
}

//---Token---


bool Token::IsRelationalOp() const
{
    return !strcmp(value, ">") || !strcmp(value, ">=") ||
           !strcmp(value, "<") || !strcmp(value, "<=") ||
           !strcmp(value, "<>") || !strcmp(value, "=");
}

bool Token::IsAddingOp() const
{
    return !strcmp(value, "+") || !strcmp(value, "-") ||
           !strcmp(value, "or") || !strcmp(value, "xor");
}

bool Token::IsMultOp() const
{
    return !strcmp(value, "*") || !strcmp(value, "/") ||
           !strcmp(value, "div") || !strcmp(value, "mod") ||
           !strcmp(value, "and") || !strcmp(value, "shl") ||
           !strcmp(value, "shr");
}

bool Token::IsUnaryOp() const
{
    return !strcmp(value, "not") || !strcmp(value, "@") ||
           !strcmp(value, "+") || !strcmp(value, "-");
}

bool Token::IsTermOp() const
{
    return !strcmp(value, "[") || !strcmp(value, ".") || !strcmp(value, "(");
}

bool Token::IsConst() const
{
    return type == HEX_CONST || type == INT_CONST || type == REAL_CONST || type == STR_CONST;
}

bool Token::IsVar() const
{
    return type == IDENTIFIER;
}

bool Token::IsConstVar() const
{
    return IsConst() || IsVar();
}

ostream& operator<<(ostream& out, const Token & token)
{
    out << token.GetLine() << ':' << token.GetPos() << ' '<< TOKEN_DESCRIPTION[token.GetType()]
            << ' ' << token.GetValue() << endl;
    return out;
}

Token::Token():
    value(NULL)
{
}

Token::Token(const char* value_, TokenType type_, int line_, int pos_):
    value(strcpy(new char[strlen(value_)+1], value_)),
    type(type_),
    line(line_),
    pos(pos_)
{
}

Token::Token(const Token& token):
    value(strcpy(new char[strlen(token.value)+1], token.value)),
	type(token.type),
	line(token.line),
	pos(token.pos)
{
}

Token& Token::operator=(const Token& token)
{
	if (value != NULL) delete(value);
	value = strcpy(new char[strlen(token.value)+1], token.value);
	type = token.type;
	line = token.line;
	pos = token.pos;
	return *this;
}

Token::~Token()
{
	if (value != NULL) delete(value);
}

TokenType Token::GetType() const
{
    return type;
}

const char* Token::GetValue() const
{
	return value;
}

int Token::GetPos() const
{
	return pos;
}

int Token::GetLine() const
{
	return line;
}

//---Scanner---

void Scanner::AddToBuffer(char c)
{
    buffer.push_back(c);
    buffer_low.push_back(tolower(c));
}

void Scanner::ReduceBuffer()
{
    buffer.resize(buffer.size() - 1);
    buffer_low.resize(buffer.size());
}

void Scanner::MakeToken(TokenType type)
{
    token = Token(buffer.c_str(), type, first_line, first_pos);
    buffer.clear();
    buffer_low.clear();
    state = NONE_ST;
}

void Scanner::IdentifyAndMake()
{
    TokenType t;
    if (!reserved_words.Identify(buffer_low, t)) t = IDENTIFIER;
    MakeToken(t);
}

bool Scanner::TryToIdentify()
{
    TokenType t;
    if (reserved_words.Identify(buffer_low, t))
    {
        MakeToken(t);
        return true;
    }
    return false;
}

void Scanner::Error(const char* msg) const
{
    stringstream s;
    s << line << ':' << pos << " ERROR " << msg;
    throw( CompilerException( s.str().c_str() ) ) ;
}

Scanner::Scanner(istream& input):
    in(input),
    line(1),
    pos(0),
    state(NONE_ST),
    c(0)
{
}

Token Scanner::GetToken()
{
	return token;
}

void Scanner::EatLineComment()
{
    if (c =='/' && in.peek() == '/')
    {
        do {
            ExtractChar();
        } while (c != '\n' && !in.eof());
    }
}

void Scanner::EatBlockComment()
{
    if (c == '{')
    {
        do {
            ExtractChar();
            if (c == '\n')
            {
                ++line;
                pos = 0;
            }
            if (in.eof()) Error("end of file in comment");
        } while (c != '}');
        ExtractChar();
    }
}

void Scanner::EatRealFractPart()
{
    while (isdigit(c))
    {
        AddToBuffer(c);
        ExtractChar();
    }
    if (c == 'e' || c == 'E')
    {
        AddToBuffer(c);
        ExtractChar();
        if (c == '+' || c == '-')
        {
            AddToBuffer(c);
            ExtractChar();
        }
        if (!isdigit(c))
            Error("illegal character, should be number");
        do
        {
            AddToBuffer(c);
            ExtractChar();
        } while (isdigit(c));
    }
    MakeToken(REAL_CONST);
}

void Scanner::EatStrNum()
{
    int res = 0;
    if (!isdigit(c)) Error("illegal char constant");
    while (isdigit(c))
    {
        res = res*10 + c - '0';
        ExtractChar();
    }
    AddToBuffer(res);
    res = 0;
    while (c == '#')
    {
        ExtractChar();
        res = 0;
        while (isdigit(c))
        {
            res = res*10 + c - '0';
            ExtractChar();
        }
        AddToBuffer(res);
    }
}

void Scanner::EatStrConst()
{
    bool isNum = (buffer[0] =='#');
    buffer.clear();
    buffer_low.clear();
    if (isNum)
    {
        EatStrNum();
        if (c == '\'')
            ExtractChar();
        else
            {
                MakeToken(STR_CONST);
                return;
            }
    }
    bool end_of_str = false;
    while (!end_of_str)
    {
        while (!end_of_str)
        {
            if (in.eof())
                Error("end of file in string");
            if (c == '\n') Error("end of line in string");
            if (c != '\'')
            {
                AddToBuffer(c);
                ExtractChar();
            }
            while (c == '\'' && !end_of_str)
            {
                int count;
                for (count = 0; c == '\'' && !in.eof(); ++count, ExtractChar())
                    if (count % 2) AddToBuffer(c);
                if (count % 2) end_of_str = true;
            }
        }
        if (c == '#')
        {
            ExtractChar();
            EatStrNum();
            if (c == '\'')
            {
                ExtractChar();
                end_of_str = false;
            }

        }
    }
    MakeToken(STR_CONST);
}

void Scanner::EatHex()
{
	bool read = false;
	while (ishexnum(c))
	{
		read = true;
		AddToBuffer(c);
		ExtractChar();
	}
	if (!read)
		Error("invalid integer expression");
	else
		MakeToken(HEX_CONST);
}

void Scanner::EatInteger()
{
    while (isdigit(c))
    {
        AddToBuffer(c);
        ExtractChar();
    }
    if (tolower(c) == 'e' || (c == '.' && in.peek() != '.'))
    {
        AddToBuffer(c);
        ExtractChar();
        EatRealFractPart();
    }
    else
        {
            MakeToken(INT_CONST);
        }
}

void Scanner::EatIdentifier()
{
    while (isalnum(c) || c == '_')
    {
        AddToBuffer(c);
        ExtractChar();
    }
    IdentifyAndMake();
}

void Scanner::EatOperation()
{
    bool matched = false;
    if (!in.eof() && !isalnum(c) && c != '_' && !isspace(c))
    {
        AddToBuffer(c);
        if (TryToIdentify())
        {
            matched = true;
            ExtractChar();
        }
        else
            {
                ReduceBuffer();
            }
    }
    if (!matched && !TryToIdentify())
        Error("illegal expression");
}

void Scanner::ExtractChar()
{
    c = in.get();
    ++pos;
}

Token Scanner::NextToken()
{
    bool matched = false;
    do
    {
        ExtractChar();
        if (state != NONE_ST) matched = true;
        switch (state)
        {
            case NONE_ST:
            break;
            case EOF_ST:
                MakeToken(END_OF_FILE);
            break;
            case INTEGER_ST:
                EatInteger();
            break;
            case HEX_ST:
                EatHex();
            break;
            case IDENTIFIER_ST:
                EatIdentifier();
            break;
            case OPERATION_ST:
                if (buffer[0] == '\'' || buffer[0] == '#')
                {
                    EatStrConst();
                }
                else
                    {
                        EatOperation();
                    };
        }
        if (state == NONE_ST)
        {
            EatLineComment();
            EatBlockComment();
            if (c == '\n')
            {
                ++line;
                pos = 0;
            }
            else
            {
                first_pos = pos;
                first_line = line;
                if (in.eof())
                {
                    state = EOF_ST;
                }
                else if (!isspace(c))
                {
                    if (isalpha(c) || c == '_')
                    {
                        state = IDENTIFIER_ST;
                    }
                    else if (isdigit(c))
                    {
                        state = INTEGER_ST;
                    }
                    else if (c == '$')
                    {
                        state = HEX_ST;
                    }
                    else
                        {
                            state = OPERATION_ST;
                        }
                    AddToBuffer(c);
                }
            }
        }
    } while (!matched);
    return token;
}

