#include "statement.h"

//---StmtAssgn---

StmtAssign::StmtAssign(SyntaxNode* left_, SyntaxNode* right_):
    left(left_),
    right(right_)
{
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
    PrintSpaces(o, offset) << ":= \n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

void StmtAssign::Generate(AsmCode& asm_code)
{
    right->GenerateValue(asm_code);
    left->GenerateLValue(asm_code);
    asm_code.MoveToMemoryFromStack(left->GetSymType()->GetSize());
}

bool StmtAssign::IsAffectToVar(SymVar* var) const
{
    return left->IsAffectToVar(var) || right->IsAffectToVar(var);
}

bool StmtAssign::IsHaveSideEffect() const
{
    return left->IsHaveSideEffect() || right->IsHaveSideEffect();
}

//---StmtBlock---

void StmtBlock::AddStatement(NodeStatement* new_stmt)
{
    if (new_stmt != NULL) statements.push_back(new_stmt);
}

void StmtBlock::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "begin\n";
    for (vector<NodeStatement*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        (*it)->Print(o, offset + 1);
    PrintSpaces(o, offset) << "end\n";
}

void StmtBlock::Generate(AsmCode& asm_code)
{
    for (vector<NodeStatement*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        (*it)->Generate(asm_code);
}

bool StmtBlock::IsAffectToVar(SymVar* var) const
{
    for (std::vector<NodeStatement*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        if ((*it)->IsAffectToVar(var)) return true;
    return false;
}

bool StmtBlock::IsHaveSideEffect() const
{
    for (std::vector<NodeStatement*>::const_iterator it = statements.begin(); it != statements.end(); ++it)
        if ((*it)->IsHaveSideEffect()) return true;
    return false;
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

void StmtExpression::Generate(AsmCode& asm_code)
{
    expr->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_ADD, expr->GetSymType()->GetSize(), REG_ESP);
}

bool StmtExpression::IsAffectToVar(SymVar* var) const
{
    return expr->IsAffectToVar(var);
}

bool StmtExpression::IsHaveSideEffect() const
{
    return expr->IsHaveSideEffect();
}

//---StmtLoop---

void StmtLoop::ObtainLabels(AsmCode& asm_code)
{
    break_label = asm_code.GenLabel("break");
    continue_label = asm_code.GenLabel("continue");;
}

StmtLoop::StmtLoop(NodeStatement* body_):
    body(body_)
{
}

AsmStrImmediate StmtLoop::GetBreakLabel() const
{
    return break_label;
}

AsmStrImmediate StmtLoop::GetContinueLabel() const
{
    return continue_label;
}

void StmtLoop::AddBody(NodeStatement* body_)
{
    if (body != NULL) delete body;
    body = body_;
}

//---StmtFor---

StmtFor::StmtFor(const SymVar* index_, SyntaxNode* init_value, SyntaxNode* last_value, bool is_inc, NodeStatement* body_):
    StmtLoop(body_),
    index(index_),
    init_val(init_value),
    last_val(last_value),
    inc(is_inc)
{
}

void StmtFor::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "for " << (inc ? "to \n" : "downto \n");
    index->PrintAsNode(o, offset + 1);
    init_val->Print(o, offset + 1);
    last_val->Print(o, offset + 1);
    body->Print(o, offset);
}

void StmtFor::Generate(AsmCode& asm_code)
{
    init_val->GenerateValue(asm_code);
    index->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_MOV, REG_EBX, AsmMemory(REG_EAX));
    AsmStrImmediate start_label(asm_code.GenLabel("for_check"));
    ObtainLabels(asm_code);
    last_val->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_JMP, continue_label, SIZE_NONE);
    asm_code.AddLabel(start_label);
    body->Generate(asm_code);
    index->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    if (inc) asm_code.AddCmd(ASM_ADD, 1, AsmMemory(REG_EAX));
    else asm_code.AddCmd(ASM_SUB, 1, AsmMemory(REG_EAX));
    asm_code.AddLabel(continue_label);
    index->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_CMP, REG_EAX, AsmMemory(REG_ESP));
    if (inc) asm_code.AddCmd(ASM_JNL, start_label, SIZE_NONE);
    else asm_code.AddCmd(ASM_JNG, start_label, SIZE_NONE);
    asm_code.AddLabel(break_label);
    asm_code.AddCmd(ASM_ADD, 4, REG_ESP);
}

bool StmtFor::IsAffectToVar(SymVar* var) const
{
    return (index == var) || body->IsAffectToVar(var) || init_val->IsAffectToVar(var)
        || last_val->IsAffectToVar(var);
}

bool StmtFor::IsHaveSideEffect() const
{
    return (index->GetClassName() & SYM_VAR_GLOBAL) || body->IsHaveSideEffect()
        || init_val->IsHaveSideEffect() || last_val->IsHaveSideEffect();
}

//---StmtWhile---

StmtWhile::StmtWhile(SyntaxNode* condition_, NodeStatement* body_):
    StmtLoop(body_),
    condition(condition_)
{
}

void StmtWhile::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "while\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

void StmtWhile::Generate(AsmCode& asm_code)
{
    ObtainLabels(asm_code);
    asm_code.AddLabel(continue_label);
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, break_label, SIZE_NONE);
    body->Generate(asm_code);
    asm_code.AddCmd(ASM_JMP, continue_label, SIZE_NONE);
    asm_code.AddLabel(break_label);
}

bool StmtWhile::IsAffectToVar(SymVar* var) const
{
    return condition->IsAffectToVar(var) || body->IsAffectToVar(var);
}

bool StmtWhile::IsHaveSideEffect() const
{
    return condition->IsHaveSideEffect() || body->IsHaveSideEffect();
}

//---StmtUntil---

StmtUntil::StmtUntil(SyntaxNode* condition_, NodeStatement* body_):
    StmtLoop(body_),
    condition(condition_)
{
}

void StmtUntil::AddCondition(SyntaxNode* condition_)
{
    condition = condition_;
}

void StmtUntil::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "until\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

void StmtUntil::Generate(AsmCode& asm_code)
{
    ObtainLabels(asm_code);
    AsmStrImmediate start_label(asm_code.GenLabel("until_start"));
    asm_code.AddLabel(start_label);
    body->Generate(asm_code);
    asm_code.AddLabel(continue_label);
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, start_label, SIZE_NONE);
    asm_code.AddLabel(break_label);
}

bool StmtUntil::IsAffectToVar(SymVar* var) const
{
    return condition->IsAffectToVar(var) || body->IsAffectToVar(var);
}

bool StmtUntil::IsHaveSideEffect() const
{
    return condition->IsHaveSideEffect() || body->IsHaveSideEffect();
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
    PrintSpaces(o, offset) << "if\n";
    condition->Print(o, offset + 1);
    PrintSpaces(o, offset) << "then\n";
    then_branch->Print(o, offset + 1);
    if (else_branch != NULL)
    {
        PrintSpaces(o, offset) << "else\n";
        else_branch->Print(o, offset + 1);
    }
}

void StmtIf::Generate(AsmCode& asm_code)
{
    bool is_const = condition->IsConst();
    bool is_true;
    if (is_const) is_true = condition->ComputeIntConstExpr();
    AsmStrImmediate label_else(asm_code.GenLabel("else"));
    AsmStrImmediate label_fin(asm_code.GenLabel("fin"));
    if (!is_const)
    {
        condition->GenerateValue(asm_code);
        asm_code.AddCmd(ASM_POP, REG_EAX);
        asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
        asm_code.AddCmd(ASM_JZ, label_else, SIZE_NONE);
    }
    if (!is_const || is_true) then_branch->Generate(asm_code);
    if (!is_const)
    {
        asm_code.AddCmd(ASM_JMP, label_fin, SIZE_NONE);
        asm_code.AddLabel(label_else);
    }
    if (else_branch != NULL && (!is_const || !is_true)) else_branch->Generate(asm_code);
    if (!is_const)
    {
        asm_code.AddCmd(ASM_JMP, label_fin, SIZE_NONE);
        asm_code.AddLabel(label_fin);
    }
}

bool StmtIf::IsAffectToVar(SymVar* var) const
{
    return condition->IsAffectToVar(var) || then_branch->IsAffectToVar(var) || else_branch->IsAffectToVar(var);
}

bool StmtIf::IsHaveSideEffect() const
{
    return condition->IsHaveSideEffect() || then_branch->IsHaveSideEffect() || else_branch->IsHaveSideEffect();
}

//---StmtJump---

StmtJump::StmtJump(Token tok, StmtLoop* loop_):
    loop(loop_),
    op(tok)
{
}

void StmtJump::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << op.GetName() << '\n';
}

void StmtJump::Generate(AsmCode& asm_code)
{
    AsmStrImmediate label = op.GetValue() == TOK_BREAK ? loop->GetBreakLabel() : loop->GetContinueLabel();
    asm_code.AddCmd(ASM_JMP, label, SIZE_NONE);
}

bool StmtJump::IsAffectToVar(SymVar* var) const
{
    return false;
}

bool StmtJump::IsHaveSideEffect() const
{
    return true;
}

//---StmtExit---

StmtExit::StmtExit(AsmStrImmediate exit_label):
    label(exit_label)
{
}

void StmtExit::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "exit\n";
}

void StmtExit::Generate(AsmCode& asm_code)
{
    asm_code.AddCmd(ASM_JMP, label, SIZE_NONE);
}

bool StmtExit::IsAffectToVar(SymVar* var) const
{
    return false;
}

bool StmtExit::IsHaveSideEffect() const
{
    return true;
}
