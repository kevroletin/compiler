#include "syntax_node.h"

static void Error(string msg, Token token)
{
    stringstream s;
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}

//---NodeCallBase---

void NodeCallBase::PrintArgs(ostream& o, int offset) const
{
    for (std::vector<SyntaxNode*>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->Print(o, offset + 1);
    }
}

void NodeCallBase::AddArg(SyntaxNode* arg)
{
    args.push_back(arg);
}

//---NodeCall---

NodeCall::NodeCall(const SymProc* funct_):
    funct(funct_)
{
}

const SymType* NodeCall::GetCurrentArgType() const
{
    if (args.size() >= funct->GetArgsCount()) return NULL;
    return funct->GetArg(args.size())->GetVarType();
}

bool NodeCall::IsCurrentArfByRef() const
{
    if (args.size() >= funct->GetArgsCount()) return NULL;
    return funct->GetArg(args.size())->IsByRef();
}

void NodeCall::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "() " << funct->GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    PrintArgs(o, offset);
}

const SymType* NodeCall::GetSymType() const
{
    return funct->GetResultType();
}

void NodeCall::GenerateValue(AsmCode& asm_code) const
{
    if (funct->GetResultType()->GetSize())
        asm_code.AddCmd(ASM_SUB, AsmImmediate(funct->GetResultType()->GetSize()), REG_ESP);
    for (int i = args.size() - 1 ; 0 <= i ; --i)
        if (funct->GetArg(i)->IsByRef())
            args[i]->GenerateLValue(asm_code);
        else
            args[i]->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_CALL, AsmMemory(funct->GetLabel()));
}

//---NodeWriteCall---

NodeWriteCall::NodeWriteCall(bool new_line_):
    new_line(new_line_)
{
}

void NodeWriteCall::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "() " << (new_line ? "writeln" : "write" ) << " [untyped]\n";
    PrintArgs(o, offset);
}

void NodeWriteCall::GenerateValue(AsmCode& asm_code) const
{
    for (std::vector<SyntaxNode*>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->GenerateValue(asm_code);
        const SymType* type = (*it)->GetSymType();
        if (type == top_type_int)
            asm_code.GenCallWriteForInt();
        else if (type == top_type_real)
            asm_code.GenCallWriteForReal();
        else
            asm_code.GenCallWriteForStr();
    }
    if (new_line) asm_code.GenWriteNewLine();
}

const SymType* NodeWriteCall::GetSymType() const
{
    return top_type_untyped;
}

//---NodeBinaryOp---

void NodeBinaryOp::FinGenForIntRelationalOp(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_CMP, REG_EBX, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_GREATER:
            asm_code.AddCmd(ASM_SETG, REG_AL, SIZE_NONE);
        break;
        case TOK_GREATER_OR_EQUAL:
            asm_code.AddCmd(ASM_SETGE, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS:
            asm_code.AddCmd(ASM_SETL, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS_OR_EQUAL:
            asm_code.AddCmd(ASM_SETLE, REG_AL, SIZE_NONE);
        break;
        case TOK_EQUAL:
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
        break;
        case TOK_NOT_EQUAL:
            asm_code.AddCmd(ASM_SETNE, REG_AL, SIZE_NONE);
    }
    asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EAX);
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeBinaryOp::FinGenForRealRelationalOp(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_FXCH, REG_ST1, SIZE_NONE);
    asm_code.AddCmd(ASM_FCOMPP, REG_ST1, SIZE_NONE);
    asm_code.AddCmd(ASM_FNSTS, REG_AX, SIZE_WORD);
    asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
    switch (token.GetValue())
    {
        case TOK_GREATER:
            asm_code.AddCmd(ASM_SETA, REG_AL, SIZE_NONE);
        break;
        case TOK_GREATER_OR_EQUAL:
            asm_code.AddCmd(ASM_SETAE, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS:
            asm_code.AddCmd(ASM_SETL, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS_OR_EQUAL:
            asm_code.AddCmd(ASM_SETLE, REG_AL, SIZE_NONE);
        break;
        case TOK_EQUAL:
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
        break;
        case TOK_NOT_EQUAL:
            asm_code.AddCmd(ASM_SETNE, REG_AL, SIZE_NONE);
    }
    asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EDX);
}

void NodeBinaryOp::GenerateForInt(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_PLUS:
            asm_code.AddCmd(ASM_ADD, REG_EBX, REG_EAX);
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_SUB, REG_EBX, REG_EAX);
        break;
        case TOK_MULT:
            asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
            asm_code.AddCmd(ASM_IMUL, REG_EBX);
        break;
        case TOK_DIV:
            asm_code.AddCmd(ASM_IDIV, REG_EBX);
        break;
        case TOK_MOD:
            asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
            asm_code.AddCmd(ASM_IDIV, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EDX);
            return;
        case TOK_AND:
            asm_code.AddCmd(ASM_AND, REG_EBX, REG_EAX);
        break;
        case TOK_OR:
            asm_code.AddCmd(ASM_OR, REG_EBX, REG_EAX);
        break;
        case TOK_XOR:
            asm_code.AddCmd(ASM_XOR, REG_EBX, REG_EAX);
        break;
        case TOK_SHR:
            asm_code.AddCmd(ASM_SAR, REG_EAX, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EBX);
            return;
        break;
        case TOK_SHL:
            asm_code.AddCmd(ASM_SAL, REG_EAX, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EBX);
            return;
        break;
        case TOK_NOT:
            asm_code.AddCmd(ASM_NOT, REG_EBX, REG_EAX);
        break;
        default:
            FinGenForIntRelationalOp(asm_code);
            return;
    }
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeBinaryOp::GenerateForReal(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP, 4), SIZE_SHORT);
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP), SIZE_SHORT);
    switch (token.GetValue())
    {
        case TOK_PLUS:
            asm_code.AddCmd(ASM_FADDP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_FSUBRP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_MULT:
            asm_code.AddCmd(ASM_FMULP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_DIVISION:
            asm_code.AddCmd(ASM_FDIVRP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        default:
            FinGenForRealRelationalOp(asm_code);
            return;
    }
    asm_code.AddCmd(ASM_ADD, 4, REG_ESP);
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

NodeBinaryOp::NodeBinaryOp(const Token& name, SyntaxNode* left_, SyntaxNode* right_):
    token(name),
    left(left_),
    right(right_)
{
}

void NodeBinaryOp::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

const SymType* NodeBinaryOp::GetSymType() const
{
    if (token.IsRelationalOp()) return top_type_int;
    return left->GetSymType();
}

void NodeBinaryOp::GenerateValue(AsmCode& asm_code) const
{
    left->GenerateValue(asm_code);
    right->GenerateValue(asm_code);
    if (GetSymType() == top_type_int) GenerateForInt(asm_code);
    else GenerateForReal(asm_code);
}

//---NodeUnaryOp---

void NodeUnaryOp::GenerateForInt(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_POP, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_NOT:
            asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
            asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EAX);
        break;
        case TOK_PLUS:
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_NEG, REG_EAX);
        break;
    }
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeUnaryOp::GenerateForReal(AsmCode& asm_code) const
{
    if (token.GetValue() != TOK_MINUS) return;
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP), SIZE_SHORT);
    asm_code.AddCmd(ASM_FCH, SIZE_SHORT);
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

NodeUnaryOp::NodeUnaryOp(const Token& name, SyntaxNode* child_):
    token(name),
    child(child_)
{
}

void NodeUnaryOp::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeUnaryOp::GetSymType() const
{
    return child->GetSymType();
}

void NodeUnaryOp::GenerateValue(AsmCode& asm_code) const
{
    child->GenerateValue(asm_code);
    if (GetSymType() == top_type_int) GenerateForInt(asm_code);
    else GenerateForReal(asm_code);
}

// NodeIntToRealConv

NodeIntToRealConv::NodeIntToRealConv(SyntaxNode* child_, SymType* real_type_):
    NodeUnaryOp(Token(), child_),
    real_type(real_type_)
{
}

void NodeIntToRealConv::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "IntToReal [";
    real_type->Print(o, 0);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeIntToRealConv::GetSymType() const
{
    return real_type;
}

void NodeIntToRealConv::GenerateValue(AsmCode& asm_code) const
{
    child->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_FILD, AsmMemory(REG_ESP), SIZE_SHORT);
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

//---NodeVar---

NodeVar::NodeVar(const SymVar* var_):
    var(var_)
{
}

const SymVar* NodeVar::GetVar()
{
    return var;
}

const SymType* NodeVar::GetSymType() const
{
    return var->GetVarType()->GetActualType();
}

void NodeVar::Print(ostream& o, int offset) const
{
    var->PrintAsNode(o, offset);
}

bool NodeVar::IsLValue() const
{
    return !(var->GetClassName() & SYM_VAR_CONST);
}

void NodeVar::GenerateLValue(AsmCode& asm_code) const
{
    var->GenerateLValue(asm_code);
}

void NodeVar::GenerateValue(AsmCode& asm_code) const
{
    var->GenerateValue(asm_code);
}

//---NodeArrayAccess----

NodeArrayAccess::NodeArrayAccess(SyntaxNode* arr_, SyntaxNode* index_):
    arr(arr_),
    index(index_)
{
}

void NodeArrayAccess::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "[] [";
    GetSymType()->Print(o, offset);
    o << "]\n";
    arr->Print(o, offset+1);
    index->Print(o, offset+1);
}

const SymType* NodeArrayAccess::GetSymType() const
{
    return ((SymTypeArray*)arr->GetSymType())->GetElemType();
}

bool NodeArrayAccess::IsLValue() const
{
    return true;
}

void NodeArrayAccess::ComputeIndexToEax(AsmCode& asm_code) const
{
    arr->GenerateLValue(asm_code);
    index->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_MOV, GetSymType()->GetSize(), REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_SUB, ((SymTypeArray*)arr->GetSymType())->GetLow(), REG_EAX);
    asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
    asm_code.AddCmd(ASM_MUL, REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_ADD, REG_EBX, REG_EAX);
}

void NodeArrayAccess::GenerateLValue(AsmCode& asm_code) const
{
    ComputeIndexToEax(asm_code);
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeArrayAccess::GenerateValue(AsmCode& asm_code) const
{
    ComputeIndexToEax(asm_code);
    if (GetSymType()->GetSize() == 4)
        asm_code.AddCmd(ASM_PUSH, AsmMemory(REG_EAX));
    else
    {
        asm_code.AddCmd(ASM_PUSH, REG_EAX);
        asm_code.PushMemory(GetSymType()->GetSize());
    }
}

//---NodeRecordAccess---

NodeRecordAccess::NodeRecordAccess(SyntaxNode* record_, Token field_):
    record(record_)
{
    const SymVarLocal* var = ((SymTypeRecord*)record_->GetSymType())->FindField(field_);
    if (var == NULL) Error("unknown record field identifier", field_);
    field = var;
}

void NodeRecordAccess::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << ". [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    record->Print(o, offset + 1);
    field->PrintAsNode(o, offset + 1);
}

const SymType* NodeRecordAccess::GetSymType() const
{
    return field->GetVarType();
}

bool NodeRecordAccess::IsLValue() const
{
    return true;
}

void NodeRecordAccess::GenerateLValue(AsmCode& asm_code) const
{
    record->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_LEA, AsmMemory(REG_EAX, field->GetOffset()), REG_EAX);
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeRecordAccess::GenerateValue(AsmCode& asm_code) const
{
    GenerateLValue(asm_code);
    asm_code.PushMemory(field->GetVarType()->GetSize());
}
