#ifndef GENERATOR
#define GENERATOR

#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <stdio.h>
using namespace std;

class AsmOperand;

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
    ASM_DIV,
    ASM_IDIV,
    ASM_IMUL,
    ASM_LEA,
    ASM_MOV,
    ASM_MUL,
    ASM_POP,
    ASM_PUSH,
    ASM_SUB,
    ASM_XOR
};

extern const string ASM_CMD_TO_STR[];

class AsmCmd{
protected:
    AsmCmdName command;
public:
    AsmCmd(AsmCmdName cmd);
    virtual void Print(ostream& o) const;
};

class AsmData{
private:
    char* name;
    unsigned size;
public:
    AsmData(string name_, unsigned size_);
    virtual void Print(ostream& o) const;    
};

class AsmCmd1: public AsmCmd{
private:
    AsmOperand* oper;
public:
    AsmCmd1(AsmCmdName cmd, AsmOperand* oper_);
    virtual void Print(ostream& o) const;    
};

class AsmCmd2: public AsmCmd{
private:
    AsmOperand* src;
    AsmOperand* dest;
public:
    AsmCmd2(AsmCmdName cmd, AsmOperand* src_, AsmOperand* dest_);
    virtual void Print(ostream& o) const;
};

class AsmOperand{
public:
    virtual void Print(ostream& o) const;    
};

class AsmOperandBase: public AsmOperand{
public:
    virtual void Print(ostream& o) const;
};

class AsmRegister: public AsmOperandBase{
private:
    RegisterName reg;
public:
    AsmRegister(RegisterName reg_);
    virtual void Print(ostream& o) const;
};

class AsmImmidiate: public AsmOperandBase{
private:
    char* value;
public:
    AsmImmidiate();
    AsmImmidiate(string value_);
    AsmImmidiate(unsigned num);
    AsmImmidiate(const AsmImmidiate& src);
    string GetValue();
    virtual void Print(ostream& o) const;
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
    vector<AsmCmd*> commands;
    vector<AsmData*> data;
    AsmImmidiate LabelByStr(string str); 
public:
    void AddCmd(AsmCmd* cmd);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper);
    void AddCmd(AsmCmdName cmd, RegisterName reg);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem);
    void AddCmd(AsmCmdName cmd, AsmImmidiate* imm);
    void AddCmd(AsmCmdName cmd, AsmImmidiate imm);
    void AddCmd(AsmCmdName cmd, AsmOperand* oper1, AsmOperand* oper2);
    void AddCmd(AsmCmdName cmd, RegisterName src, RegisterName dest);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate* dest);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmImmidiate dest);
    void AddCmd(AsmCmdName cmd, AsmImmidiate* src, RegisterName oper1);
    void AddCmd(AsmCmdName cmd, AsmImmidiate src, RegisterName oper1);
    void AddCmd(AsmCmdName cmd, AsmMemory* mem, RegisterName reg);
    void AddCmd(AsmCmdName cmd, RegisterName reg, AsmMemory* mem);
    void AddData(AsmData* new_data);
    AsmImmidiate AddData(string label, unsigned size);
    virtual void Print(ostream& o) const;
};

#endif
