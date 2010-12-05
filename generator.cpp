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
    "call",
    "div",
    "idiv",
    "imul",
    "lea",
    "mov",
    "mul",
    "pop",
    "push",
    "ret",
    "sub",
    "xor"
};

const string ASM_DATA_TYPE_TO_STR[] = 
{
    ".space",
    ".long",
    ".float",
    ".string"
};

//---AsmCmd---

AsmCmd::AsmCmd(AsmCmdName cmd):
    command(cmd)
{
}

void AsmCmd::Print(ostream& o) const
{
    o << ASM_CMD_TO_STR[command];
}

//---AsmData---

AsmData::AsmData(string name_, string value_, AsmDataType type):
    name(strcpy(new char[name_.size() + 1], name_.c_str())),
    value(strcpy(new char[value_.size() + 1], value_.c_str())),
    type(type)
{
}

void AsmData::Print(ostream& o) const
{
    o << name << ": " << ASM_DATA_TYPE_TO_STR[type] << ' ';
    if (type == DATA_STR) o << '\"' << value << '\"';
    else o << value;
}

//---AsmCmd1---

AsmCmd1::AsmCmd1(AsmCmdName cmd, AsmOperand* oper_):
    AsmCmd(cmd),
    oper(oper_)
{
}

void AsmCmd1::Print(ostream& o) const
{
    o << ASM_CMD_TO_STR[command] << "l\t";
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
    o << ASM_CMD_TO_STR[command] << "l\t";
    src->Print(o);
    o<< ", ";
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
    o << REG_TO_STR[reg];
}

//---AsmImmidiate---

AsmImmidiate::AsmImmidiate():
    value(strcpy(new char[2], ""))
{
}

AsmImmidiate::AsmImmidiate(const string& value_):
    value(strcpy(new char[value_.size() + 1], value_.c_str()))
{
}

AsmImmidiate::AsmImmidiate(unsigned num)
{
    value = new char[15];
    sprintf(value, "%u", num);
}

AsmImmidiate::AsmImmidiate(const AsmImmidiate& src)
{
    if (value != NULL) delete(value);
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

AsmMemory::AsmMemory(AsmImmidiate base_, unsigned disp_, unsigned index_, unsigned scale_):
    base(new AsmImmidiate(base_)),
    disp(disp_),
    index(index_),
    scale(scale_)
{
}

AsmMemory::AsmMemory(RegisterName reg, unsigned disp_, unsigned index_, unsigned scale_):
    base(new AsmRegister(reg)),
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

AsmCode::AsmCode()
//    format_str_real(AddData("format_str_f", "%d", DATA_STR))
//    format_str_int(AddData("format_str_d", "%d", DATA_STR))
//    funct_write(AsmMemory(AsmImmidiate("printf")))
{
    AddData(ChangeName("format_str_f"), "%d", DATA_STR);
    AddData(ChangeName("format_str_d"), "%d", DATA_STR);
//    AsmMemory(AsmImmidiate("printf"));
}

AsmImmidiate AsmCode::LabelByStr(string str)
{
    return AsmImmidiate(ChangeName(str));
}

string AsmCode::ChangeName(string str)
{
    return str;
}

void AsmCode::AddCmd(AsmCmd* cmd)
{
    commands.push_back(cmd);
}

void AsmCode::AddCmd(AsmCmdName cmd, AsmMemory mem)
{
    commands.push_back(new AsmCmd1(cmd, new AsmMemory(mem)));
}

void AsmCode::AddCmd(AsmCmdName cmd, RegisterName src, RegisterName dest)
{
    commands.push_back(new AsmCmd2(cmd, new AsmRegister(src), new AsmRegister(dest)));
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

void AsmCode::AddData(string label, string value, AsmDataType type)
{
    string new_name = ChangeName(label); 
    data.push_back(new AsmData(new_name, value, type));
}

void AsmCode::AddData(string label, unsigned size)
{
    char buff[15];
    sprintf(buff, "%u", size);
    data.push_back(new AsmData(ChangeName(label), buff));
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
    o << ".globl main\nmain:\n";
    for (vector<AsmCmd*>::const_iterator it = commands.begin(); it != commands.end(); ++it)
    {
        o << '\t';
        (*it)->Print(o);
        o << '\n';
    }
    o << "\tret\n";
}

void AsmCode::CallWriteForInt()
{
 //   AddCmd(ASM_PUSH, format_str_int);
 //   AddCmd(ASM_CALL, funct_write);
 //   AddCmd(ASM_ADD, AsmImmidiate(8), REG_ESP);
}

void AsmCode::CallWriteForReal()
{
 //   AddCmd(ASM_PUSH, format_str_real);
 //   AddCmd(ASM_CALL, funct_write);
 //   AddCmd(ASM_ADD, AsmImmidiate(8), REG_ESP);
}
