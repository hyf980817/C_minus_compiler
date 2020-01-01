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
        //printf("creating op_var, id: %s\n", name);
        op->name = name;
    case OP_CONST:
        op->int_val = val;
        break;
    case OP_TEMP:
        //printf("creating op_temp\n");
    case OP_LABEL:
        op->no_val = val;
        break;
    default:
        printf("Unknown Op Type in function: createOperand_INT\n");
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
        printf("%d", op->int_val);
        break;
    case OP_VAR:
        printf("%s", op->name);
        break;
    case OP_TEMP:
        printf("t%d", op->no_val);
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
        printf(" := ");
        printOperand(code->assign.right);
        printf("\n");
        break;
    case I_GOTO:
        printf("GOTO Label%d\n", code->unary.op->no_val);
        break;
    case I_ADD:
        printOperand(code->binop.result);
        printf(" := ");
        printOperand(code->binop.op1);
        printf(" + ");
        printOperand(code->binop.op2);
        printf("\n");
        break;
    default:
        printf("Unknown code\n");
        break;
    }
}


//初始化中间代码段
InterCodes initNewInterCodes()
{
    InterCodes codes = (InterCodes)malloc(sizeof(struct InterCodes_));
    codes->code_seg = NULL;
    codes->r_brother = NULL;
    codes->child = NULL;

    return codes;
}

//向中间代码段中添加一条中间代码
void addInterCode(InterCodes codes, InterCode code)
{   
    InterCode end = codes->code_seg;

    if(end == NULL)
    {
        codes->code_seg = code;
        return;
    }

    while(end->next != NULL)
    {
        end = end->next;
    }

    end->next = code;
}

void PrintInterCodes(InterCodes codes)
{
    if(codes->child != NULL)
    {
        //printf("In codes\n");
        assert(codes->code_seg == NULL);
        InterCodes child = codes->child;
        while(child != NULL)
        {
            //printf("In printing child\n");
            PrintInterCodes(child);
            child = child->r_brother;
        }
    }
    else
    {
        //printf("In printging code seg\n");
        assert(codes->code_seg != NULL);
        InterCode code = codes->code_seg;
        while(code != NULL)
        {
            //printf("In printing code\n");
            printInterCode(code);
            code = code->next;
        }
    }
    
}