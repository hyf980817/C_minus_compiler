#ifndef _INTERCODE_H_
#define _INTERCODE_H_
struct Operand_ {
    enum {OP_VAR, OP_CONST, OP_ADDR, OP_TEMP ,OP_LABEL} kind;
    union {
        int int_val;
        float float_val;
        char char_val;
        int no_val;
    };
    char *name;
};

typedef struct Operand_* Operand;

struct InterCode_
{
    enum {I_LABEL, I_FUNCTION, I_ASSIGN, I_ADD, I_SUB, I_MUL, 
    I_DIV, I_GOTO, I_IFGOTO, I_RETURN, I_ARG, I_DEC, I_PARAM} kind;
    union 
    {
        struct {Operand right, left;} assign;
        struct {Operand result, op1, op2;} binop;
        struct {Operand op;} unary;
    };
};

typedef struct InterCode_* InterCode;

struct InterCodes_
{
    InterCode code;
    struct InterCodes_ *pre;
    struct InterCodes_ *next;
};

typedef struct InterCodes_ InterCodes;



Operand createOperand_INT(int type, int val, char *name);

InterCode createInterCode_ASSIGN(Operand src, Operand dst);

InterCode createInterCode_BINOP(Operand dst, Operand src1, Operand src2, int type);

InterCode createInterCode_UNARY(Operand op, int type);

void printInterCode(InterCode code);

#endif
