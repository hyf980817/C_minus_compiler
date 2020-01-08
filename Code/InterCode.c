#include "InterCode.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "syntax.tab.h"

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

//创建函数定义的中间语句
InterCode createInterCode_FUNDEF(char *name, int return_type)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = I_FUNDEF;
    code->fundef.name = name;
    code->fundef.return_type = return_type;
    code->next = NULL;
    return code;
}

InterCode createInterCode_CALL(Operand result, char *name)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = I_CALL;
    code->call.result = result;
    code->call.name = name;
    code->next = NULL;
    return code;
}

InterCode createInterCode_PARAM(Operand param)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = I_PARAM;
    code->param.op = param;
    code->next = NULL;
    return code;
}



InterCode createInterCode_LABEL(Operand label)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = I_LABEL;
    code->param.op = label;
    code->next = NULL;
    return code;
}

InterCode createInterCode_IFGOTO(Operand left, Operand right, Operand label, int relop)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = I_IFGOTO;
    code->ifgoto.label = label;
    code->ifgoto.right = right;
    code->ifgoto.left = left;
    code->ifgoto.relop = relop;
    code->next = NULL;
    return code;    
}



//创建二元操作数的表达式中间语句
InterCode createInterCode_BINOP(Operand dst, Operand src1, Operand src2, int code_type, int op_type)
{
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    code->kind = code_type;
    code->binop.op1 = src1;
    code->binop.op2 = src2;
    code->binop.result = dst;
    code->next = NULL;
    code->binop.op_type = op_type;
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
        printf("#%d", op->int_val);
        break;
    case OP_VAR:
        printf("%s", op->name);
        break;
    case OP_TEMP:
        printf("$t%d", op->no_val);
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
    case I_DEREF:
        printOperand(code->assign.left);
        printf(" := *");
        printOperand(code->assign.right);
        printf("\n");
        break;        
    case I_GOTO:
        printf("GOTO Label%d\n", code->unary.op->no_val);
        break;
    case I_BINOP:
        printOperand(code->binop.result);
        printf(" := ");
        printOperand(code->binop.op1);
        switch (code->binop.op_type)
        {
        case OP_ADD:
            printf(" + ");
            break;
        case OP_SUB:
            printf(" - ");
            break;
        case OP_STAR:
            printf(" * ");
            break;
        case OP_DIV:
            printf(" / ");
            break;
        case OP_SHL:
            printf(" << ");
            break;
        case OP_SHR:
            printf(" >> ");
            break;
        case OP_BIT_AND:
            printf(" & ");
            break;
        case OP_BIT_OR:
            printf(" | ");
            break;  
        case OP_BIT_XOR:
            printf(" ^ ");
            break;  
        default:
            printf("Unkown binop!\n");
            break;
        }
        printOperand(code->binop.op2);
        printf("\n");
        break;
    case I_FUNDEF:
        printf("\nFunction %s : \n", code->fundef.name);
        break;
    case I_PARAM:
        printf("PARAM %s\n", code->param.op->name);
        break;
    case I_ARG:
        printf("ARG ");
        printOperand(code->unary.op);
        printf("\n");
        break;
    case I_CALL:
        if(code->call.result == NULL)
            printf("CALL Function %s\n", code->call.name);
        else{
            printOperand(code->call.result);
            printf(" := CALL Function %s\n", code->call.name);
        }
        break;
    case I_RETURN:
        printf("Return t%d\n", code->unary.op->int_val);
        break;

    case I_LABEL:
        printf("LABEL %d:\n", code->label.op->int_val);
        break;
    case I_IFGOTO:
        printf("IF ");
        printOperand(code->ifgoto.left);
        switch (code->ifgoto.relop)
        {
        case OP_GT:
            printf(" > ");
            break;
        case OP_LT:
            printf(" < ");
            break;
        case OP_GE:
            printf(" >= ");
            break;
        case OP_LE:
            printf(" <= ");
            break;
        case OP_EQ:
            printf(" == ");
            break;
        case OP_NEQ:
            printf(" != ");
            break;
        default:
            printf(" ?? ");
            break;
        }
        printOperand(code->ifgoto.right);
        printf(" GOTO LABEL%d\n", code->ifgoto.label->int_val);
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
    
    assert(codes->code_seg == NULL || codes->child == NULL);

    while(codes->r_brother != NULL)
        codes = codes->r_brother;

    InterCode end = codes->code_seg;
    if(end == NULL && codes->child == NULL)
    {
        codes->code_seg = code;
    }

    if(end != NULL && codes->child == NULL){
        while(end->next != NULL)
        {
            end = end->next;
        }
        end->next = code;
    }

    if(end == NULL && codes->child != NULL)
    {
        addInterCode(codes->child, code);
    }

}

void addInterCodesAsChild(InterCodes codes, InterCodes newcodes)
{
    assert(codes != NULL && newcodes != NULL);
    InterCodes child = codes->child;
    if(child == NULL)
        codes->child = newcodes;
    else
    {
        while(child->r_brother != NULL)
            child = child->r_brother;
        child->r_brother = newcodes;
    }
    
}

void PrintInterCodes(InterCodes codes)
{
    if(codes->child != NULL)
    {
        //printf("In codes\n");
        if(codes->code_seg != NULL)
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
        if(codes->code_seg != NULL)
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