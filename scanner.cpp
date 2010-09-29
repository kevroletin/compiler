#include "scanner.h"

const char* const TOKEN_DESCRIPTION[] =
{
	"IDENTIFIER",
	"RESERVED_WORD",
	"HEX_CONST",
	"INT_CONST",
	"REAL_CONST",
	"STR_CONST",
	"STRING_CONST",
	"OPERATION",
	"DELIMITER",
	"END_OF_FILE"
};

//---Reserved words--

void ReservedWords::Add(char* value, TokenType type)
{
    words.insert(pair<char*, TokenType>(value, type));
}

ReservedWords::ReservedWords()
{
    Add("absolute", RESERVED_WORD);
    Add("and", OPERATION);
    Add("array", RESERVED_WORD);
    Add("asm", RESERVED_WORD);
    Add("begin", RESERVED_WORD);
    Add("case", RESERVED_WORD);
    Add("const", RESERVED_WORD);
    Add("constructor", RESERVED_WORD);
    Add("destructor", RESERVED_WORD);
    Add("div", OPERATION);
    Add("do", RESERVED_WORD);
    Add("downto", RESERVED_WORD);
    Add("else", RESERVED_WORD);
    Add("end", RESERVED_WORD);
    Add("file", RESERVED_WORD);
    Add("for", RESERVED_WORD);
    Add("function", RESERVED_WORD);
    Add("goto", RESERVED_WORD);
    Add("if", RESERVED_WORD);
    Add("implementation", RESERVED_WORD);
    Add("in", RESERVED_WORD);
    Add("inherited", RESERVED_WORD);
    Add("inline", RESERVED_WORD);
    Add("interface", RESERVED_WORD);
    Add("label", RESERVED_WORD);
    Add("mod", OPERATION);
    Add("nil", RESERVED_WORD);
    Add("not", OPERATION);
    Add("object", OPERATION);
    Add("of", RESERVED_WORD);
    Add("or", OPERATION);
    Add("on", RESERVED_WORD);
    Add("operator", RESERVED_WORD);
    Add("packed", RESERVED_WORD);
    Add("procedure", RESERVED_WORD);
    Add("program", RESERVED_WORD);
    Add("record", RESERVED_WORD);
    Add("repeat", RESERVED_WORD);
    Add("reintroduce", RESERVED_WORD);
    Add("set", RESERVED_WORD);
    Add("self", RESERVED_WORD);
    Add("shl", OPERATION);
    Add("shr", OPERATION);
    Add("string", RESERVED_WORD);
    Add("then", RESERVED_WORD);
    Add("to", RESERVED_WORD);
    Add("type", RESERVED_WORD);
    Add("unit", RESERVED_WORD);
    Add("until", RESERVED_WORD);
    Add("uses", RESERVED_WORD);
    Add("var", RESERVED_WORD);
    Add("while", RESERVED_WORD);
    Add("with", RESERVED_WORD);
    Add("xor", OPERATION);
    Add("..", RESERVED_WORD);
    Add(":=", OPERATION);
    Add("-", OPERATION);
    Add("+", OPERATION);
    Add("*", OPERATION);
    Add("/", OPERATION);
    Add("[", OPERATION);
    Add("]", OPERATION);
    Add(";", DELIMITER);
    Add(":", DELIMITER);
    Add(",", DELIMITER);
    Add(".", OPERATION);
    Add("^", OPERATION);
    Add("@", OPERATION);
    Add("(", OPERATION);
    Add(")", OPERATION);
    Add(">", OPERATION);
    Add("<", OPERATION);
    Add("=", OPERATION);
    Add(">=", OPERATION);
    Add("<=", OPERATION);
    Add("<>", OPERATION);
}

bool ReservedWords::Identify(char *value, TokenType& returned_type)
{
    map<string, TokenType>::iterator i  = words.find(value);
    if (i == words.end()) return false;
    returned_type = i->second;
    return true;
}

//---Token---

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

Token::Token(char* value, TokenType type, int line, int pos)
{
    this->value = strcpy(new char[strlen(value)+1], value);
	this->type = type;
	this->line = line;
	this->pos = pos;
}

Token::Token(const Token& token)
{
    value = strcpy(new char[strlen(token.value)+1], token.value);
	type = token.type;
	line = token.line;
	pos = token.pos;
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
	delete(value);
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

Scanner::exception::exception():
    msg(NULL)
{
}

Scanner::exception::exception(const char* const msg):
    msg(strcpy(new char[strlen(msg) + 1], msg))
{
}

const char* Scanner::exception::what() const throw()
{
    return msg;
}

//---Scanner---

void Scanner::AddToBuffer(char c)
{
    buffer[bp] = c;
    buffer_low[bp++] = tolower(c);
}

void Scanner::MakeToken(TokenType type)
{
    buffer[bp] = '\0';
    token = Token(buffer, type, first_line, first_pos);
    bp = 0;
    state = NONE_ST;
}

void Scanner::IdentifyAndMake()
{
    buffer_low[bp]  = '\0';
    TokenType t;
    if (!reserved_words.Identify(buffer_low, t)) t = IDENTIFIER;
    MakeToken(t);
}

bool Scanner::TryToIdentify()
{
    buffer_low[bp]  = '\0';
    TokenType t;
    if (reserved_words.Identify(buffer_low, t))
    {
        MakeToken(t);
        return true;
    }
    else
        return false;
}

void Scanner::Error(const char* msg) const
{
    stringstream s;
    s << line << ':' << pos << " ERROR " << msg;
    throw( Scanner::exception( s.str().c_str() ) ) ;
}

Scanner::Scanner(istream& input):
    in(input),
    bp(0),
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
    if (c =='/' && (char)in.peek() == '/')
    {
        do {
            c = in.get();
            ++pos;
        } while (c != '\n' && !in.eof());
    }
}

void Scanner::EatBlockComment()
{
    if (c == '{')
    {
        do {
            c = in.get();
            ++pos;
            if (c == '\n')
            {
                ++line;
                pos = 0;
            }
            if (in.eof()) Error("end of file in comment");
        } while (c != '}');
        c = in.get();
        ++pos;
    }
}

void Scanner::EatRealFractPart()
{
    while (isdigit(c))
    {
        AddToBuffer(c);
        c = ExtractChar();
    }
    if (c == 'e' || c == 'E')
    {
        AddToBuffer(c);
        c = ExtractChar();
        if (c == '+' || c == '-')
        {
            AddToBuffer(c);
            c = ExtractChar();
        }
        if (!isdigit(c))
            Error("illegal character, should be number");
        else
            do
            {
                AddToBuffer(c);
                c = ExtractChar();
            } while (isdigit(c));
    }
    MakeToken(REAL_CONST);
}

void Scanner::EatStrConst()
{
    --bp;
    while (c != '\'')
    {
        if (in.eof())
            Error("end of file in string");
        if (c != '\n')
            AddToBuffer(c);
        else if (buffer[bp - 1] != '\\')
            Error("end of line in string");
        else
            {
                ++line;
                pos = 0;
                --bp;
            }
        int count = 0;
        c = ExtractChar();
        while (c == '\'' && !in.eof())
        {
            ++count;
            if (count % 2 == 0) AddToBuffer(c);
            c = in.get();
            ++pos;
        }
        if (count % 2) break;
    }
    if (!in.eof())
    {
        c = ExtractChar();
    }
    MakeToken(STR_CONST);
}

void Scanner::EatHex()
{
	bool read = false;
	while (isdigit(c) || ('a' <= tolower(c)  && tolower(c) <= 'f'))
	{
		read = true;
		AddToBuffer(c);
		c = ExtractChar();
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
        c = ExtractChar();
    }
    if (c == '.')
    {
        AddToBuffer(c);
        c = ExtractChar();
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
        c = ExtractChar();
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
            c = ExtractChar();
        }
        else
            --bp;
    }
    if (!matched)
    {
        if (TryToIdentify())
            matched = true;
        else
            Error("illegal expression");
    }
}

char Scanner::ExtractChar()
{
    ++pos;
    char c = in.get();
    return c;
}

Token Scanner::NextToken()
{
    bool matched;
    do
    {
        if (!in.eof())
        {
            c = ExtractChar();
        }
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
                if (buffer[0] == '\'')
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

