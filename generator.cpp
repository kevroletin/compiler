#include "generator.h"

const string REG_TO_STR[] =
{
    "%eax",
    "%ebx",
    "%ecx",
    "%edx",
    "%edi",
    "%esi",
    "%ebp",
    "%esp"
};

const string ASM_CMD_TO_STR[] =
{
    "add",
    "div",
    "lea",
    "mov",
    "mul",
    "pop",
    "push",
    "sub"
};


//---AsmCmd---

AsmCmd::AsmCmd(AsmCmdName cmd):
    command(cmd)
{
}

void AsmCmd::Print(ostream& o) const
{
    o << ASM_CMD_TO_STR[command] << "l ";
}

//---AsmData---

AsmData::AsmData(string name_, unsigned size_):
    name(strcpy(new char[name_.size() + 1], name_.c_str())),
    size(size_)
{

}

void AsmData::Print(ostream& o) const
{
    o << name << ": .space " << size;
}

//---AsmCmd1---

AsmCmd1::AsmCmd1(AsmCmdName cmd, AsmOperand* oper_):
    AsmCmd(cmd),
    oper(oper_)
{
}

void AsmCmd1::Print(ostream& o) const
{
    o << ASM_CMD_TO_STR[command] << "l ";
    oper->Print(o);
}

//---AsmCmd2---

AsmCmd2::AsmCmd2(AsmCmdName cmd, AsmOperand* src_, AsmOperand* dest_):
    AsmCmd(cmd),
    src(src_),
    dest(dest_)
{
}

void AsmCmd2::Print(ostream& o) const
{
    o << ASM_CMD_TO_STR[command] << "l ";
    src->Print(o);
    dest->Print(o);
}

//---AsmOperand---

void AsmOperand::Print(ostream& o) const
{
}

//---AsmOperandBase---

void AsmOperandBase::Print(ostream& o) const
{
}

//---AsmRegister---

AsmRegister::AsmRegister(RegisterName reg_):
    reg(reg_)
{
}

void AsmRegister::Print(ostream& o) const
{
    o << REG_TO_STR[reg] << ' ';
}

//---AsmImmidiate---

AsmImmidiate::AsmImmidiate():
    value(strcpy(new char[1], ""))
{
}

AsmImmidiate::AsmImmidiate(string value_):
    value(strcpy(new char[value_.size() + 1], value_.c_str()))
{
}

AsmImmidiate::AsmImmidiate(const AsmImmidiate& src)
{
    delete(value);
    value = strcpy(new char[strlen(src.value) + 1], src.value);
}

void AsmImmidiate::Print(ostream& o) const
{
    o << '$' << value;
}

string AsmImmidiate::GetValue()
{
    return value;
}

//---AsmMemory---

AsmMemory::AsmMemory(AsmOperandBase* base_, unsigned disp_, unsigned index_, unsigned scale_):
    base(base_),
    disp(disp_),
    index(index_),
    scale(scale_)
{
}

void AsmMemory::Print(ostream& o) const
{
    if (disp) o << disp;
    o << '(';
    base->Print(o);
    if (index) o << ", " << index;
    if (scale) o << ", " << scale;
    o << ')';
}    

//---AsmCode---

AsmImmidiate AsmCode::LabelByStr(string str)
{
    return AsmImmidiate(str);
}

void AsmCode::AddCmd(AsmCmd* cmd)
{
    commands.push_back(cmd);
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmOperand* oper)
{
    commands.push_back(new AsmCmd1(cmd, oper));
}

void AsmCode::AddCmd(AsmCmdName cmd, RegisterName reg)
{
    commands.push_back(new AsmCmd1(cmd, new AsmRegister(reg)));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmMemory* mem)
{
    commands.push_back(new AsmCmd1(cmd, mem));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmImmidiate* imm)
{
    commands.push_back(new AsmCmd1(cmd, imm));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmImmidiate imm)
{
    AsmImmidiate* tmp = new AsmImmidiate(imm);
    commands.push_back(new AsmCmd1(cmd, tmp));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmOperand* oper1, AsmOperand* oper2)
{
    commands.push_back(new AsmCmd2(cmd, oper1, oper2));
}
        
void AsmCode::AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate* dest)
{
    commands.push_back(new AsmCmd2(cmd, new AsmRegister(reg), dest));
}

void AsmCode::AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate dest)
{
    AsmImmidiate* imm = new AsmImmidiate(dest);
    commands.push_back(new AsmCmd2(cmd, new AsmRegister(reg), imm));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmImmidiate* src, RegisterName reg)
{
    commands.push_back(new AsmCmd2(cmd, src, new AsmRegister(reg)));
}


void AsmCode::AddCmd(AsmCmdName cmd, AsmImmidiate src, RegisterName reg)
{
    AsmImmidiate* imm = new AsmImmidiate(src);
    commands.push_back(new AsmCmd2(cmd, imm, new AsmRegister(reg)));
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmMemory* mem, RegisterName reg)
{
    commands.push_back(new AsmCmd2(cmd, mem, new AsmRegister(reg)));
}

void AsmCode::AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory* mem)
{
    commands.push_back(new AsmCmd2(cmd, new AsmRegister(reg), mem));
}

void AsmCode::AddData(AsmData* new_data)
{
    data.push_back(new_data);
}

AsmImmidiate AsmCode::AddData(string label, unsigned size)
{
    AsmImmidiate imm = LabelByStr(label);
    data.push_back(new AsmData(imm.GetValue(), size));
    return imm;
}

void AsmCode::Print(ostream& o) const
{
    o << ".data\n";
    for (vector<AsmData*>::const_iterator it = data.begin(); it != data.end(); ++it)
    {
        o << '\t';
        (*it)->Print(o);
        o << '\n';
    }
    o << ".text\n";
    for (vector<AsmCmd*>::const_iterator it = commands.begin(); it != commands.end(); ++it)
    {
        o << '\t';
        (*it)->Print(o);
        o << '\n';
    }
}