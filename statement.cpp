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

//---StmtBlock---

void StmtBlock::AddStatement(NodeStatement* new_stmt)
{
    statements.push_back(new_stmt);
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

AsmImmidiate StmtLoop::GetBreakLabel() const
{
    return break_label;
}

AsmImmidiate StmtLoop::GetContinueLabel() const
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
    AsmImmidiate start_label(asm_code.GenLabel("for_check"));
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
    AsmImmidiate start_label(asm_code.GenLabel("until_start"));
    asm_code.AddLabel(start_label);
    body->Generate(asm_code);
    asm_code.AddLabel(continue_label);
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, start_label, SIZE_NONE);
    asm_code.AddLabel(break_label);
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
    AsmImmidiate label_else(asm_code.GenLabel("else"));
    AsmImmidiate label_fin(asm_code.GenLabel("fin"));
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, label_else, SIZE_NONE);
    then_branch->Generate(asm_code);
    asm_code.AddCmd(ASM_JMP, label_fin, SIZE_NONE);
    asm_code.AddLabel(label_else);
    if (else_branch != NULL) else_branch->Generate(asm_code);
    asm_code.AddCmd(ASM_JMP, label_fin, SIZE_NONE);
    asm_code.AddLabel(label_fin);    
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
    AsmImmidiate label = op.GetValue() == TOK_BREAK ? loop->GetBreakLabel() : loop->GetContinueLabel();
    asm_code.AddCmd(ASM_JMP, label, SIZE_NONE);
}

//---StmtExit---

StmtExit::StmtExit(AsmImmidiate exit_label):
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
