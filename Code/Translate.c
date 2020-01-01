#include "InterCode.h"
#include "Tree.h"
#include <assert.h>
#include "syntax.tab.h"
#define GET_TEMP 0
#define FREE_TEMP 1

int manage_temp(int mode, int no)
{
    static int temp[32] = {0};
    if(mode == GET_TEMP)  //获取一个temp寄存器
    {
        int i;
        for(i = 0; i < 32; i++)
        {
            if(temp[i] == 0)
            {
                temp[i] = 1;
                return i;
            }
        }
        assert(i != 32);
    }
    else  //释放一个temp寄存器
    {
        assert(no >= 0 && no < 32);
        assert(temp[no] == 1);
        temp[no] == 0;
    }
    return 0;
}

//翻译表达式
InterCodes translate_Exp(T* Exp, RBRoot* tables[], int depth, Operand place)
{
    InterCodes codes = initNewInterCodes();
    T* child = Exp->child;

    if(child->type[0] == 'I')   //Expr->ID || Expr->INT
    {
        //printf("In ID or INT\n");
        Operand op = NULL;
        if(child->type_no == INT)  
        {
            op = createOperand_INT(OP_CONST, child->int_val,NULL);
        }
        if(child->type_no == ID)
        {
            //printf("child->id:%s", child->id);
            op = createOperand_INT(OP_VAR, 0, child->id);
        }
        InterCode code = createInterCode_ASSIGN(op, place);
        addInterCode(codes, code);
    }
    if(child->type[0] == 'E')  //二元运算或数组   Expr-> Expr OP_XXXX Expr   || Expr-> Expr LB Expr RB
    {
        //printf("In Expr\n");
        T* Expr1 = child;
        T* OP = Expr1->r_brother;
        assert(Expr1->type_no == Expr);
        assert(OP->type[0] == 'O'); //目前暂时不考虑数组
        T* Expr2 = OP->r_brother;
        //赋值表达式的翻译
        if(OP->type_no == OP_ASSIGN)
        {
            //printf("In OP_ASSIGN\n");
            int t1 = manage_temp(GET_TEMP, 0);
            Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
            Operand variable = createOperand_INT(OP_VAR, 0, Expr1->child->id);
            
            InterCodes code1 = translate_Exp(Expr2, tables, depth, temp1);  //首先将Expr2的结果赋值给temp1
            
            InterCodes code2 = initNewInterCodes();
            InterCode c1 = createInterCode_ASSIGN(temp1, variable);
            InterCode c2 = createInterCode_ASSIGN(variable, place);
            addInterCode(code2, c1);
            addInterCode(code2, c2);

            code1->r_brother = code2;
            codes->child = code1;
            manage_temp(FREE_TEMP, t1); //释放temp寄存器
            
        }

        //Expr1 + Expr2的翻译
        if(OP->type_no == OP_ADD)
        {
            //printf("In OP\n");
            int t1 = manage_temp(GET_TEMP, 0);
            int t2 = manage_temp(GET_TEMP, 0);
            Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
            Operand temp2 = createOperand_INT(OP_TEMP, t2, NULL);

            InterCodes code1 = translate_Exp(Expr1, tables, depth, temp1);
            InterCodes code2 = translate_Exp(Expr2, tables, depth, temp2);
            InterCodes code3 = initNewInterCodes();
            InterCode c1 = createInterCode_BINOP(place, temp1, temp2, I_ADD);
            addInterCode(code3, c1);
            code2->r_brother = code3;
            code1->r_brother = code2;
            codes->child = code1;
            manage_temp(FREE_TEMP, t1);
            manage_temp(FREE_TEMP, t2);
        }
        
    }
    return codes;
}
