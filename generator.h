#ifndef GENERATOR
#define GENERATOR

#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <sstream>
using namespace std;

class AsmOperand;

enum CmdSize{
    SIZE_NONE,
    SIZE_BYTE,
    SIZE_SHORT,
    SIZE_LONG,
    SIZE_QUARD
};

extern const string SIZE_TO_STR[];

enum RegisterName{
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_EDI,
    REG_ESI,
    REG_EBP,
    REG_ESP
};

extern const string REG_TO_STR[];

enum AsmCmdName{
    ASM_ADD,
    ASM_CALL,
    ASM_DIV,
    ASM_IDIV,
    ASM_IMUL,
    ASM_LEA,
    ASM_MOV,
    ASM_MUL,
    ASM_POP,
    ASM_PUSH,
    ASM_RET,
    ASM_SUB,
    ASM_XOR

    ,
    ASM_FILD,
    ASM_FLD,
    ASM_FSTP
};

extern const string ASM_CMD_TO_STR[];

enum AsmDataType{
    DATA_UNTYPED,
    DATA_INT,
    DATA_REAL,
    DATA_STR
};

extern const string ASM_DATA_TYPE_TO_STR[];

class AsmCmd{
protected:
    AsmCmdName command;
    CmdSize size;
public:
    AsmCmd(AsmCmdName cmd, CmdSize cmd_size = SIZE_LONG);
    virtual void Print(ostream& o) const;
};

class AsmData{
private:
    string name;
    string value;
    AsmDataType type;
public:
    AsmData(string name_, string value, AsmDataType type = DATA_UNTYPED);
    virtual void Print(ostream& o) const;    
};

class AsmCmd1: public AsmCmd{
private:
    AsmOperand* oper;
public:
    AsmCmd1(AsmCmdName cmd, AsmOperand* oper_, CmdSize size = SIZE_LONG);
    virtual void Print(ostream& o) const;    
};

class AsmCmd2: public AsmCmd{
private:
    AsmOperand* src;
    AsmOperand* dest;
public:
    AsmCmd2(AsmCmdName cmd, AsmOperand* src_, AsmOperand* dest_, CmdSize size = SIZE_LONG);
    virtual void Print(ostream& o) const;
};

class AsmOperand{
public:
    virtual void Print(ostream& o) const;    
};

class AsmOperandBase: public AsmOperand{
public:
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmRegister: public AsmOperandBase{
private:
    RegisterName reg;
public:
    AsmRegister(RegisterName reg_);
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmImmidiate: public AsmOperandBase{
private:
    string value;
public:
    AsmImmidiate();
    AsmImmidiate(const string& value_);
    AsmImmidiate(unsigned num);
    AsmImmidiate(const AsmImmidiate& src);
    string GetValue();
    virtual void Print(ostream& o) const;
    virtual void PrintBase(ostream& o) const;
};

class AsmMemory: public AsmOperand{
private:
    AsmOperandBase* base;
    unsigned disp;
    unsigned index;
    unsigned scale;
public:
    AsmMemory(AsmOperandBase* base_, unsigned disp_ = 0, unsigned index_ = 0, unsigned scale_ = 0);
    AsmMemory(AsmImmidiate base, unsigned disp_ = 0, unsigned index_ = 0, unsigned scale_ = 0);
    AsmMemory(RegisterName reg, unsigned disp_ = 0, unsigned index_ = 0, unsigned scale_ = 0);
    virtual void Print(ostream& o) const;
};

class AsmCode{
private:
    AsmImmidiate format_str_real;
    AsmImmidiate format_str_int;
    AsmMemory funct_write; 
    vector<AsmCmd*> commands;
    vector<AsmData*> data;
    AsmImmidiate LabelByStr(string str);
    string ChangeName(string str);
    unsigned label_counter;
public:
    AsmCode();
    AsmImmidiate GenLabel();
    string GenStrLabel();
    AsmImmidiate GenLabel(string prefix);
    string GenStrLabel(string prefix);
    void AddCmd(AsmCmd* cmd);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper);
    void AddCmd(AsmCmdName cmd, RegisterName reg);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem);
    void AddCmd(AsmCmdName cmd, AsmMemory mem, CmdSize size = SIZE_LONG);
    void AddCmd(AsmCmdName cmd, AsmImmidiate* imm);
    void AddCmd(AsmCmdName cmd, AsmImmidiate imm);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper1, AsmOperand* oper2);
    void AddCmd(AsmCmdName cmd, RegisterName src, RegisterName dest);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate* dest);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate dest);
    void AddCmd(AsmCmdName cmd, AsmImmidiate* src, RegisterName oper1);
    void AddCmd(AsmCmdName cmd, AsmImmidiate src, RegisterName oper1);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem, RegisterName reg);
    void AddCmd(AsmCmdName cmd, AsmMemory mem, RegisterName reg);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory* mem);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory mem);
    void AddCmd(AsmCmdName cmd, AsmImmidiate* src, AsmMemory* mem);
    void AddCmd(AsmCmdName cmd, AsmImmidiate src, AsmMemory mem);    
    void AddData(AsmData* new_data);
    AsmImmidiate AddData(string label, string value, AsmDataType type = DATA_UNTYPED);
    AsmImmidiate AddData(string label, unsigned size);
    AsmImmidiate AddData(string value, AsmDataType type = DATA_UNTYPED);
    AsmImmidiate AddData(unsigned size);
    virtual void Print(ostream& o) const;
    void CallWriteForInt();
    void CallWriteForReal();
};

#endif
