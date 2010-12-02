#include "syntax_node.h"

void Error(string msg, Token token)
{
    stringstream s;
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}

void PrintSpaces(ostream& o, int offset)
{
    for (int i = 0; i < offset; ++i)
        o << "  ";
}

//---SyntaxNode---

SyntaxNode* SyntaxNode::ConvertType(SyntaxNode* node, const SymType* type)
{
    if (node->GetSymType() == type) return node;
    if (node->GetSymType() == top_type_int && type == top_type_real)
        return new NodeIntToRealConv(node, top_type_real);
    return NULL;
}

void SyntaxNode::TryToConvertType(SyntaxNode*& first, SyntaxNode*& second)
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

void SyntaxNode::TryToConvertType(SyntaxNode*& expr, const SymType* type)
{
    if (expr->GetSymType() == type) return;
    SyntaxNode* tmp = ConvertType(expr, type);
    if (tmp == NULL) return;
    expr = tmp;
    return;
}

void SyntaxNode::TryToConvertTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err)
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

void SyntaxNode::TryToConvertTypeOrDie(SyntaxNode*& expr, const SymType* type, Token tok_err)
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


void SyntaxNode::Print(ostream& o, int offset) const 
{
}

const SymType* SyntaxNode::GetSymType() const
{
    return NULL;
}

bool SyntaxNode::IsLValue() const
{
    return false;
}

//---NodeStatement---

//---StmtAssgn---

StmtAssign::StmtAssign(Token& op, SyntaxNode* left_, SyntaxNode* right_):
    left(left_),
    right(right_)
{
    TryToConvertTypeOrDie(right, left->GetSymType(),  op);
    if (left->GetSymType() != right->GetSymType()) Error("incompatible types", op);
    if (!(left->IsLValue())) Error("l-value expected", op);
}

const SyntaxNode* StmtAssign::GetLeft() const
{
    return left;
}

const SyntaxNode* StmtAssign::GetRight() const
{
    return right;
}

void StmtAssign::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << ":= \n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

//---StmtBlock---

void StmtBlock::AddStatement(SyntaxNode* new_stmt)
{
    statements.push_back(new_stmt);
}

void StmtBlock::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "begin\n";
    for (vector<SyntaxNode*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        (*it)->Print(o, offset + 1);
    PrintSpaces(o, offset);
    o << "end;\n";
}

//---NodeCall---

NodeCall::NodeCall(const SymProc* funct_):
    funct(funct_)
{
}
    
void NodeCall::AddArg(SyntaxNode* arg)
{ 
    args.push_back(arg);
}

void NodeCall::ValidateParamsList()
{
    //TODO
}

void NodeCall::Print(ostream& o, int offset) const 
{
    funct->Print(o, offset);
    o << funct->GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    for (std::vector<SyntaxNode*>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->Print(o, offset + 1);
    }
}

const SymType* NodeCall::GetSymType() const
{
    return funct->GetResultType();
}

//---NodeBinaryOp---

NodeBinaryOp::NodeBinaryOp(const Token& name, SyntaxNode* left_, SyntaxNode* right_):
    token(name),
    left(left_),
    right(right_)
{
    TryToConvertTypeOrDie(left, right, name);
}

void NodeBinaryOp::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

const SymType* NodeBinaryOp::GetSymType() const
{
    return left->GetSymType();
}

//---NodeUnaryOp---

NodeUnaryOp::NodeUnaryOp(const Token& name, SyntaxNode* child_):
    token(name),
    child(child_)
{
}

void NodeUnaryOp::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeUnaryOp::GetSymType() const
{
    return child->GetSymType();
}

// NodeIntToRealConv

NodeIntToRealConv::NodeIntToRealConv(SyntaxNode* child_, SymType* real_type_):
    NodeUnaryOp(Token(), child_),
    real_type(real_type_)     
{
}

void NodeIntToRealConv::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << "IntToReal [";
    real_type->Print(o, 0);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeIntToRealConv::GetSymType() const
{
    return real_type;
}

//---NodeVar---

NodeVar::NodeVar(SymVar* var_):
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
    return !(var->GetClassName() & (SYM_VAR_CONST | SYM_VAR_PARAM));
}

//---NodeArrayAccess----

NodeArrayAccess::NodeArrayAccess(SyntaxNode* arr_, SyntaxNode* index_, Token tok):
    arr(arr_),
    index(index_)
{
    if (!(arr->GetSymType()->GetClassName() & SYM_TYPE_ARRAY)) Error("array expected before '[' token", tok);
    if (index->GetSymType() != top_type_int) Error("index of array must be integer", tok);
}

void NodeArrayAccess::Print(ostream& o, int offset) const 
{
    PrintSpaces(o, offset);
    o << "[] [";
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

//---NodeRecordAccess---

NodeRecordAccess::NodeRecordAccess(SyntaxNode* record_, Token field_):
    record(record_)
{
    if (!(record_->GetSymType()->GetClassName() & SYM_TYPE_RECORD))
        Error("illegal qualifier", field_);
    const SymVar* var = ((SymTypeRecord*)record_->GetSymType())->FindField(field_);
    if (var == NULL) Error("unknown record field identifier", field_);
    field = var;
}

void NodeRecordAccess::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << ". [";
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
