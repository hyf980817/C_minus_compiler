#include "InterCode.h"
#include "Tree.h"
#include <assert.h>
#include "syntax.tab.h"
#define GET_TEMP 0
#define FREE_TEMP 1


//管理temp类型的寄存器, 可以获取或者释放
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

//管理跳转编号
int manage_label()
{
    static int label = 0;
    return label++;
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
            addInterCode(code2, c1);
            
            if(place != NULL)
            {
                InterCode c2 = createInterCode_ASSIGN(variable, place);
                addInterCode(code2, c2);
            }

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
            if(place != NULL)
            {
                InterCode c1 = createInterCode_BINOP(place, temp1, temp2, I_ADD);
                addInterCode(code3, c1);
                code2->r_brother = code3;
            }
            code1->r_brother = code2;
            codes->child = code1;
            manage_temp(FREE_TEMP, t1);
            manage_temp(FREE_TEMP, t2);
        }
        
    }
    return codes;
}


InterCodes translate_Stmt(T* stmt, RBRoot* tables[], int depth)
{
    assert(stmt != NULL);
    T* child = stmt->child;
    if(child->type_no == Expr)
        return translate_Exp(child, tables, depth, NULL);


}

//翻译整个程序
/*
*Program
*    DefList
*        VarDefStmt
*            ...
*        DefList
*            FunDef
*                ...
*            DefList
*                FunDef
*                    ...
*/


//翻译参数语句, 即可以翻译函数定义时的参数的定义, 也可以翻译函数调用时的传入参数
//取决于mode
InterCodes translate_args(T* lp, int mode, RBRoot* tables[], int depth)
{
    //lp初始指向左括号
    if(I_ARG == mode)
    {
        T* args = lp->r_brother;
        if(args == NULL) //无参数调用
            return NULL;
        InterCodes codes = initNewInterCodes();
        T* expr = args->child;
        while(expr != NULL)
        {
            T* child = expr->child;
            Operand op;
            switch (child->type_no)
            {
            case INT:
                op = createOperand_INT(OP_CONST, child->int_val, NULL);     
                break;
            case ID:
                op = createOperand_INT(OP_VAR, 0, child->id);
                break;
            default:
                break;
            }
            InterCode code = createInterCode_PARAM(op);
            addInterCode(codes, code);

            //让expr指向下一个参数
            expr = expr->r_brother;
            if(expr != NULL){
                expr = expr->r_brother;
                assert(expr != NULL);
                expr = expr->child;
            }
        }
        return codes;
    }
    if(I_PARAM == mode)
    {
        T* paralist = lp->r_brother;
        if(paralist == NULL) //无参数调用
            return NULL;
        InterCodes codes = initNewInterCodes();
        T* para_dec = paralist->child;
        while(para_dec != NULL)
        {
            T* var_id = para_dec->child->r_brother->child;
            Operand op = createOperand_INT(OP_VAR, 0, var_id->id);
            InterCode code = createInterCode_PARAM(op);
            addInterCode(codes, code);

            //让para_dec指向下一个参数定义
            para_dec = para_dec->r_brother;
            if(para_dec != NULL){
                para_dec = para_dec->r_brother;
                assert(para_dec != NULL);
                para_dec = para_dec->child;
            }
        }
        return codes;
    }
    return NULL;
}


//翻译block
InterCodes translate_block(T* block, RBRoot* tables[], int depth)
{
    
}



InterCodes translate_Program(T* program, RBRoot* tables[], int depth)
{
    assert(program != NULL);
    T* deflist = program->child;
    InterCodes codes = initNewInterCodes();
    while(deflist != NULL)
    {
        T* child = deflist->child;
        if(child->type_no == VarDecList)
            ; //全局变量语句, 暂时跳过
        if(child->type_no == FunDef)
        {
            //翻译函数定义
            T* return_type = child->child;
            T* fun_dec = return_type->r_brother;
            T* fun_id = fun_dec->child; 
            InterCodes codes_func = initNewInterCodes();
            InterCode function = createInterCode_FUNDEF(fun_id->id, return_type->child->type_no);
            addInterCode(codes_func, function);
            addInterCodesAsChild(codes, codes_func);
            //翻译参数
            T* lp = fun_id->r_brother;
            InterCodes codes_params = translate_args(lp, I_PARAM, tables, depth);

            //翻译block
            T* block = fun_dec->r_brother;

            
        }
        deflist = child->r_brother;
    }

    return codes;
}
