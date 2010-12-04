#include "syntax_node.h"

//---NodeCall---

NodeCall::NodeCall(const SymProc* funct_):
    funct(funct_)
{
}
    
void NodeCall::AddArg(SyntaxNode* arg)
{ 
    args.push_back(arg);
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
    PrintSpaces(o, offset);
    o << "() " << funct->GetName() << " [";
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
    return !(var->GetClassName() & SYM_VAR_CONST);
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

//---StmtAssgn---

StmtAssign::StmtAssign(Token& op, SyntaxNode* left_, SyntaxNode* right_):
    left(left_),
    right(right_)
{
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
    o << "end\n";
}

//---StmtExpression---

StmtExpression::StmtExpression(SyntaxNode* expression):
    expr(expression)
{
}

void StmtExpression::Print(ostream& o, int offset) const
{
    expr->Print(o, offset);
}

//---StmtFor---

StmtFor::StmtFor(const SymVar* index_, SyntaxNode* init_value, SyntaxNode* last_value, bool is_inc, NodeStatement* body_):
    index(index_),
    init_val(init_value),
    last_val(last_value),
    inc(is_inc),
    body(body_)
{
}

void StmtFor::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "for " << (inc ? "to \n" : "downto \n");
    index->PrintAsNode(o, offset + 1);
    init_val->Print(o, offset + 1);
    last_val->Print(o, offset + 1);
    body->Print(o, offset);
}

//---StmtWhile---

StmtWhile::StmtWhile(SyntaxNode* condition_, NodeStatement* body_):
    condition(condition_),
    body(body_)
{
}
    
void StmtWhile::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "while\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

//---StmtUntil---

StmtUntil::StmtUntil(SyntaxNode* condition_, NodeStatement* body_):
    condition(condition_),
    body(body_)
{
}

void StmtUntil::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "until\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

//---StmtIf---

StmtIf::StmtIf(SyntaxNode* condition_, NodeStatement* then_branch_, NodeStatement* else_branch_):
    condition(condition_),
    then_branch(then_branch_),
    else_branch(else_branch_)
{
}

void StmtIf::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset);
    o << "if\n";
    condition->Print(o, offset + 1);
    PrintSpaces(o, offset);
    o << "then\n";
    then_branch->Print(o, offset + 1);
    if (else_branch != NULL)
    {
        PrintSpaces(o, offset);
        o << "else\n";
        else_branch->Print(o, offset + 1);
    }
}
