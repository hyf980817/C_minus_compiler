#include "InterCode.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//创建一个操作数
Operand createOperand_INT(int type, int val, char *name)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = type;
    switch (type)
    {
    case OP_VAR:
        op->name = name;
    case OP_CONST:
        op->int_val = val;
        break;
    case OP_TEMP:
    case OP_LABEL:
        op->no_val = val;
        break;
    default:
        fprintf(stderr, "Unknown Op Type in function: createOperand_INT\n");
        break;
    }

    return op;
}



//创建赋值表达式的中间语句
InterCode createInterCode_ASSIGN(Operand src, Operand dst)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->assign.left = dst;
    code->assign.right = src;
    code->kind = I_ASSIGN;
    code->next = NULL;
    return code;
}


//创建二元操作数的表达式中间语句
InterCode createInterCode_BINOP(Operand dst, Operand src1, Operand src2, int type)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    assert(type <= I_PARAM && type >= I_LABEL);
    code->kind = type;
    code->binop.op1 = src1;
    code->binop.op2 = src2;
    code->binop.result = dst;
    code->next = NULL;
    return code;
}

//创建一元的表达式的中间语句
InterCode createInterCode_UNARY(Operand op, int type)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    assert(type <= I_PARAM && type >= I_LABEL);
    code->kind = type;
    code->unary.op = op;
    code->next = NULL;
    return code; 
}

//打印一个操作数
void printOperand(Operand op)
{
    switch (op->kind)
    {
    case OP_CONST:
        fprintf(stdout, "%d", op->int_val);
        break;
    case OP_VAR:
        fprintf(stdout, "%s", op->name);
        break;
    case OP_TEMP:
        fprintf(stdout , "t%d", op->no_val);
        break;
    default:
        break;
    }
}

//打印一条中间代码
void printInterCode(InterCode code)
{
    switch (code->kind)
    {
    case I_ASSIGN:
        printOperand(code->assign.left);
        fprintf(stdout, " := ");
        printOperand(code->assign.right);
        fprintf(stdout, "\n");
        break;
    case I_GOTO:
        fprintf(stdout, "GOTO Label%d\n", code->unary.op->no_val);
        break;
    case I_ADD:
        printOperand(code->binop.result);
        fprintf(stdout, " := ");
        printOperand(code->binop.op1);
        fprintf(stdout, " + ");
        printOperand(code->binop.op2);
        fprintf(stdout, "\n");
        break;
    default:
        break;
    }
}


//初始化中间代码段
InterCodes initNewInterCodes()
{
    InterCodes codes;
    codes.code_seg = NULL;
    codes.next = NULL;
    codes.pre = NULL;

    return codes;
}

//向中间代码段中添加一条中间代码
void addInterCode(InterCodes codes, InterCode code)
{   
    InterCode end = codes.code_seg;

    if(end == NULL)
    {
        codes.code_seg = code;
        return;
    }

    while(end->next != NULL)
    {
        end = end->next;
    }

    end->next = code;
}