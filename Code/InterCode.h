#ifndef _INTERCODE_H_
#define _INTERCODE_H_
struct Operand_ {
    enum {OP_VAR = 4096, OP_CONST, OP_ADDR, OP_TEMP ,OP_LABEL, OP_FUNC} kind;
    union {
        int int_val;
        float float_val;
        char char_val;
        int no_val;
    };
    char *name;
};

typedef struct Operand_* Operand;


//单条指令, 但是next指针可以指向这条指令的下一条指令, 所以事实上能表示一串指令
struct InterCode_
{
    enum {I_ASSIGN = 8192, I_ADD, I_GOTO, I_IFGOTO, I_FUNDEF, I_CALL, I_ARG, I_PARAM, I_LABEL, I_RETURN} kind;
    union 
    {
        struct {Operand right, left;} assign; //I_ASSIGN, I_CALL
        struct {Operand result, op1, op2;} binop; //I_ADD
        struct {Operand op;} unary;  //I_GOTO, I_RETURN, I_ARG
        struct {Operand left, right, label; int relop;} ifgoto;  //I_IFGOTO
        struct {char *name; int return_type;} fundef;
        struct {Operand op;} param;
        struct {Operand op;} label;

        
    };
    struct InterCode_* next;
};

typedef struct InterCode_* InterCode;

//树型IR的节点
struct InterCodes_
{
    InterCode code_seg; //中间代码, 唯有叶子节点这里才为非空.
    struct InterCodes_ *child;
    struct InterCodes_ *r_brother;
};

typedef struct InterCodes_ *InterCodes;



Operand createOperand_INT(int type, int val, char *name);

InterCode createInterCode_ASSIGN(Operand src, Operand dst);
InterCode createInterCode_BINOP(Operand dst, Operand src1, Operand src2, int type);
InterCode createInterCode_UNARY(Operand op, int type);
InterCode createInterCode_FUNDEF(char *name, int return_type);
InterCode createInterCode_PARAM(Operand param);
InterCode createInterCode_LABEL(Operand label);
InterCode createInterCode_IFGOTO(Operand left, Operand right, Operand label, int relop);


void printInterCode(InterCode code);


InterCodes initNewInterCodes();
void addInterCode(InterCodes codes, InterCode code);
void addInterCodesAsChild(InterCodes codes, InterCodes newcodes);


void PrintInterCodes(InterCodes codes);
#endif
