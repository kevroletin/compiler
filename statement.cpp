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

void StmtAssign::Generate(AsmCode& asm_code) const
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

void StmtBlock::Generate(AsmCode& asm_code) const
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

void StmtExpression::Generate(AsmCode& asm_code) const
{
    expr->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_ADD, expr->GetSymType()->GetSize(), REG_ESP);
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
    PrintSpaces(o, offset) << "for " << (inc ? "to \n" : "downto \n");
    index->PrintAsNode(o, offset + 1);
    init_val->Print(o, offset + 1);
    last_val->Print(o, offset + 1);
    body->Print(o, offset);
}

void StmtFor::Generate(AsmCode& asm_code) const
{
    init_val->GenerateValue(asm_code);
    index->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_MOV, REG_EBX, AsmMemory(REG_EAX));
    AsmImmidiate start_label(asm_code.GenLabel("for_check"));
    AsmImmidiate check_label(asm_code.GenLabel("for_check"));
    AsmImmidiate fin_label(asm_code.GenLabel("for_fin"));
    last_val->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_JMP, check_label, SIZE_NONE);
    asm_code.AddLabel(start_label);
    body->Generate(asm_code);
    index->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    if (inc) asm_code.AddCmd(ASM_ADD, 1, AsmMemory(REG_EAX));
    else asm_code.AddCmd(ASM_SUB, 1, AsmMemory(REG_EAX));
    asm_code.AddLabel(check_label);
    index->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_CMP, REG_EAX, AsmMemory(REG_ESP));
    if (inc) asm_code.AddCmd(ASM_JNL, start_label, SIZE_NONE);
    else asm_code.AddCmd(ASM_JNG, start_label, SIZE_NONE);
    asm_code.AddLabel(fin_label);
    asm_code.AddCmd(ASM_ADD, 4, REG_ESP);
}

//---StmtWhile---

StmtWhile::StmtWhile(SyntaxNode* condition_, NodeStatement* body_):
    condition(condition_),
    body(body_)
{
}
    
void StmtWhile::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "while\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

void StmtWhile::Generate(AsmCode& asm_code) const
{
    AsmImmidiate label_check(asm_code.GenLabel("while_check"));
    AsmImmidiate label_fin(asm_code.GenLabel("while_fin"));
    asm_code.AddLabel(label_check);
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, label_fin, SIZE_NONE);
    body->Generate(asm_code);
    asm_code.AddCmd(ASM_JMP, label_check, SIZE_NONE);
    asm_code.AddLabel(label_fin);
}

//---StmtUntil---

StmtUntil::StmtUntil(SyntaxNode* condition_, NodeStatement* body_):
    condition(condition_),
    body(body_)
{
}

void StmtUntil::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "until\n";
    condition->Print(o, offset + 1);
    body->Print(o, offset + 1);
}

void StmtUntil::Generate(AsmCode& asm_code) const
{
    AsmImmidiate label_start(asm_code.GenLabel("until_check"));
    asm_code.AddLabel(label_start);
    body->Generate(asm_code);
    condition->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
    asm_code.AddCmd(ASM_JZ, label_start, SIZE_NONE);
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

void StmtIf::Generate(AsmCode& asm_code) const
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

void StmtJump::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << name;
}

void StmtJump::Generate(AsmCode& asm_code) const
{
    asm_code.AddCmd(AsmLabel(label));
}
