#include "Tree.h"
#include <assert.h>
#include "syntax.tab.h"
#include <stdio.h>

InterCodes translate_block(T* block, RBRoot* tables[], int depth);
InterCodes translate_Exp(T* expr, RBRoot* tables[], int depth, Operand place);
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
    label++;
    return label;
}

//翻译数组
InterCodes translate_Array(T* id, RBRoot* tables[], int depth, Operand place)
{
    Node *id_node = search_tables(id->id, tables, depth); //找到符号表中这个id
    symbol value = id_node->value;  


    //int dimension = value.array[0];   //数组维度
    //assert(dimension != 0);

    int t1 = manage_temp(GET_TEMP, 0);
    int t2 = manage_temp(GET_TEMP, 0);
    Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
    Operand temp2 = createOperand_INT(OP_TEMP, t2, NULL);
    Operand four = createOperand_INT(OP_CONST, 4, NULL);
    Operand var = createOperand_INT(OP_VAR, 0, id->id);

    T* expr = id->r_brother->r_brother;
    assert(expr->type_no == Expr);
    //计算索引大小
    InterCodes codes_index = translate_Exp(expr, tables, depth, temp1);
    
    //索引乘以4
    InterCode code_four = createInterCode_BINOP(temp2, temp1, four, I_BINOP, OP_STAR);
    //基址
    InterCode code_base = createInterCode_ASSIGN(var, temp1);
    //索引加基址
    InterCode code_addr = createInterCode_BINOP(temp1, temp1, temp2, I_BINOP, OP_ADD);

    //解引用
    InterCode code_deref = createInterCode_ASSIGN(temp1,place);
    code_deref->kind = I_DEREF;

    addInterCode(codes_index, code_four);
    addInterCode(codes_index, code_base);
    addInterCode(codes_index, code_addr);
    addInterCode(codes_index, code_deref);
    manage_temp(FREE_TEMP, t1);
    manage_temp(FREE_TEMP, t2);
    return codes_index;
}

//翻译表达式
InterCodes translate_Exp(T* Exp, RBRoot* tables[], int depth, Operand place)
{
    InterCodes codes = initNewInterCodes();
    T* child = Exp->child;

    if(child->type[0] == 'I')   //Expr->ID || Expr->INT || Expr-> ID LP RP || Expr-> ID LP Args RP
    {
        //printf("In ID or INT\n");
        Operand op = NULL;
        switch (child->type_no)
        {
        case INT:
            op = createOperand_INT(OP_CONST, child->int_val,NULL);
            InterCode code = createInterCode_ASSIGN(op, place);
            addInterCode(codes, code);
            break;
        case ID:
            if(child->r_brother == NULL) //纯ID
            {
                op = createOperand_INT(OP_VAR, 0, child->id);
                InterCode code = createInterCode_ASSIGN(op, place);
                addInterCode(codes, code);
                break;
            }
            //函数调用
            if(child->r_brother->type_no == LP)
            {
                T* fundec = child;
                T* lp = child->r_brother;
                if(lp->r_brother->type_no != RP) //有参数, 先计算参数
                {
                    T* args = lp->r_brother;
                    int t_start = -1; //存储获取的temp寄存器, 方便后面释放
                    int t_end = -1;
                    InterCodes codes_args = initNewInterCodes();
                    while(args != NULL)
                    {
                        T* expr = args->child;
                        int t = manage_temp(GET_TEMP, 0); //获取存储结果的temp寄存器
                        if(t_start == -1)
                            t_start = t;
                        t_end = t;
                        Operand temp = createOperand_INT(OP_TEMP, t, NULL);
                        //翻译参数的expr
                        InterCodes codes_expr = translate_Exp(expr, tables, depth, temp);
                        addInterCodesAsChild(codes, codes_expr);
                        //创建ARG语句
                        InterCode code_arg = createInterCode_UNARY(temp, I_ARG);
                        addInterCode(codes_args, code_arg);
                        //准备读取下一个arg  Args->Expr COMMA Args
                        args = args->r_brother;
                        if(args != NULL)
                        {
                            args = args->r_brother;
                        }
                    }
                    
                    for(int i = t_start; i <= t_end; i++) {
                        manage_temp(FREE_TEMP, i);
                    }
                    addInterCodesAsChild(codes, codes_args);
                }
                
                InterCode code_function = createInterCode_CALL(place, fundec->id);
                addInterCode(codes, code_function);
            }
            if(child->r_brother->type_no == LB) //数组
            {
                codes = translate_Array(child, tables, depth,place);
            }
            break;
        default:
            break;
        }
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
            int t1 = manage_temp(GET_TEMP, 0);  //获取一个temp寄存器
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
        //二元操作的翻译
        if(OP->type_no >= OP_STAR && OP->type_no <=OP_BIT_OR)
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
                InterCode c1 = createInterCode_BINOP(place, temp1, temp2, I_BINOP, OP->type_no);
                addInterCode(code3, c1);
                code2->r_brother = code3;
            }
            code1->r_brother = code2;
            codes->child = code1;
            manage_temp(FREE_TEMP, t1);
            manage_temp(FREE_TEMP, t2);
        }
        
    }

    if(child->type[0] == 'O') //一元运算
    {
        switch (child->type_no)
        {
        case OP_SUB:{  //负数
            int t1 = manage_temp(GET_TEMP, 0);
            Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
            Operand zero = createOperand_INT(OP_CONST, 0, NULL);

            codes = translate_Exp(child->r_brother, tables, depth, temp1);
            if(place != NULL)
            {
                InterCode c1 = createInterCode_BINOP(place, zero, temp1, I_BINOP, OP_SUB);
                addInterCode(codes, c1);
            }
            manage_temp(FREE_TEMP, t1);
            break;
        }
        default:
            break;
        }
    }
    return codes;
}

//翻译bool表达式, 包括关系运算符, AND, OR
InterCodes translate_Condition(T* expr, Operand label_true, Operand label_false, RBRoot* tables[], int depth)
{
    assert(expr != NULL);
    InterCodes codes = initNewInterCodes();
    if(expr->child->type_no == Expr)
    {
        T* expr1 = expr->child;
        T* op = expr1->r_brother;

        T* expr2 = op->r_brother;
        switch (op->type_no)
        {
        //比较运算符
        case OP_GE:
        case OP_LE:
        case OP_GT:
        case OP_LT:
        case OP_EQ:
        case OP_NEQ:{
            int t1 = manage_temp(GET_TEMP, 0);
            int t2 = manage_temp(GET_TEMP, 0);
            Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
            Operand temp2 = createOperand_INT(OP_TEMP, t2, NULL);
            InterCodes codes1 = translate_Exp(expr1, tables, depth, temp1);
            InterCodes codes2 = translate_Exp(expr2, tables, depth, temp2);
            InterCode code_ifgoto = createInterCode_IFGOTO(temp1, temp2, label_true, op->type_no);
            InterCode code_goto_false = createInterCode_UNARY(label_false, I_GOTO);
            addInterCode(codes2, code_ifgoto);
            addInterCode(codes2, code_goto_false);
            addInterCodesAsChild(codes, codes1);
            addInterCodesAsChild(codes, codes2);
            manage_temp(FREE_TEMP, t2);
            manage_temp(FREE_TEMP, t1);
            return codes;
        }
        // bool
        case OP_AND:{
            int l1 = manage_label();
            Operand label1 = createOperand_INT(OP_LABEL, l1, NULL);
            InterCodes codes1 = translate_Condition(expr1, label1, label_false, tables, depth);
            InterCodes codes2 = translate_Condition(expr2, label_true, label_false, tables, depth);
            InterCodes codes = initNewInterCodes();
            InterCode code_label1 = createInterCode_LABEL(label1);
            addInterCode(codes1, code_label1);
            addInterCodesAsChild(codes, codes1);
            addInterCodesAsChild(codes, codes2);
            return codes;
        }
        case OP_OR:{
            int l1 = manage_label();
            Operand label1 = createOperand_INT(OP_LABEL, l1, NULL);
            InterCodes codes1 = translate_Condition(expr1, label_true, label1, tables, depth);
            InterCodes codes2 = translate_Condition(expr2, label_true, label_false, tables, depth);
            InterCode code_label1 = createInterCode_LABEL(label1);
            addInterCode(codes1, code_label1);
            addInterCodesAsChild(codes, codes1);
            addInterCodesAsChild(codes, codes2);
            return codes;
        }
        default:
            printf("Unknown OP!!!\n");
            break;
        }

    }
    else //只有一个expr, 通过这个expr的值来判断是否跳转
    {
        int t1 = manage_temp(GET_TEMP, 0);
        Operand temp1 = createOperand_INT(OP_TEMP, t1, NULL);
        Operand temp2 = createOperand_INT(OP_CONST, 0, NULL);
        InterCodes codes1 = translate_Exp(expr, tables, depth, temp1);
        InterCode code_ifgoto = createInterCode_IFGOTO(temp1, temp2, label_true, OP_NEQ);
        InterCode code_goto_false = createInterCode_UNARY(label_false, I_GOTO);
        addInterCode(codes1, code_ifgoto);
        addInterCode(codes1, code_goto_false);
        InterCodes codes = initNewInterCodes();
        addInterCodesAsChild(codes, codes1);
        manage_temp(FREE_TEMP, t1);
        return codes;   
    }
    
    return NULL;
}


//翻译一条语句
InterCodes translate_Stmt(T* stmt, RBRoot* tables[], int depth)
{
    assert(stmt != NULL);
    InterCodes codes = initNewInterCodes();
    T* child = stmt->child;

    if(child->type_no == Expr) //Stmt-> Expr SEMI
        return translate_Exp(child, tables, depth, NULL);


    if(child->type_no == BLOCK) {  //Stmt -> BLOCK
        tables[++depth] = child->table;
        return translate_block(child, tables, depth--);
    }


    if(child->type_no == RETURN) { //Stmt -> RETURN Expr SEMI
        int t1 = manage_temp(GET_TEMP, 0);
        Operand temp = createOperand_INT(OP_TEMP, t1, NULL);
        T* expr = child->r_brother;
        assert(expr != NULL);
        codes = translate_Exp(expr, tables, depth, temp);
        InterCode return_code = createInterCode_UNARY(temp, I_RETURN);
        addInterCode(codes, return_code);
        manage_temp(FREE_TEMP, t1);
    }


    if(child->type_no == IF) //if语句, 两种情况, if(expr) stmt 和 if(expr) stmt else stmt
    {
        T* If = child;
        assert(child->r_brother != NULL);
        assert(child->r_brother->r_brother != NULL);
        assert(child->r_brother->r_brother->r_brother != NULL);
        assert(child->r_brother->r_brother->r_brother->r_brother != NULL);

        T* first_stmt= child->r_brother->r_brother->r_brother->r_brother;
        T* expr = If->r_brother->r_brother;
        if(first_stmt->r_brother == NULL) //if(expr) stmt
        {
            //获取两个label
            int l1 = manage_label();
            int l2 = manage_label();
            Operand label1 = createOperand_INT(OP_LABEL, l1, NULL);
            Operand label2 = createOperand_INT(OP_LABEL, l2, NULL);
            //TODO : 翻译bool表达式
            InterCodes codes1 =translate_Condition(expr, label1, label2, tables, depth);
            InterCodes codes2 = translate_Stmt(first_stmt, tables, depth);
            InterCode code_label1 = createInterCode_LABEL(label1);
            InterCode code_label2 = createInterCode_LABEL(label2);
            //printf("\n\n");
            //PrintInterCodes(codes1);
            //printf("\n\n");
            addInterCode(codes1, code_label1);
            //printf("\n\n");
            //PrintInterCodes(codes1);
            //printf("\n\n");
            addInterCode(codes2, code_label2);

            addInterCodesAsChild(codes, codes1);
            addInterCodesAsChild(codes, codes2);
            //return codes1 + code_label1 + codes2 + code_label2
        }
        else
        {
            T* second_stmt = first_stmt->r_brother->r_brother;
            //获取label
            int l1 = manage_label();
            int l2 = manage_label();
            int l3 = manage_label();
            Operand label1 = createOperand_INT(OP_LABEL, l1, NULL);
            Operand label2 = createOperand_INT(OP_LABEL, l2, NULL);
            Operand label3 = createOperand_INT(OP_LABEL, l3, NULL);
     

            InterCodes codes1 =translate_Condition(expr, label1, label2, tables, depth);
            InterCodes codes2 = translate_Stmt(first_stmt, tables, depth);
            InterCodes codes3 = translate_Stmt(second_stmt, tables, depth);

            InterCode code_label1 = createInterCode_LABEL(label1);
            InterCode code_label2 = createInterCode_LABEL(label2);
            InterCode code_label3 = createInterCode_LABEL(label3);
            InterCode goto_label3 = createInterCode_UNARY(label3, I_GOTO);
            addInterCode(codes1, code_label1);
            addInterCode(codes2, goto_label3);
            addInterCode(codes2, code_label2);
            addInterCode(codes3, code_label3);
            //return codes1 + code_label1 + codes2 + goto_label3 + code_label2 + codes3 + code_label3;
            addInterCodesAsChild(codes, codes1);
            addInterCodesAsChild(codes, codes2);
            addInterCodesAsChild(codes, codes3);
        }
        
    }

    if(child->type_no == WHILE)  //while循环
    {
        T* expr = child->r_brother->r_brother;
        T* stmt = expr->r_brother->r_brother;
        int l1 = manage_label();
        int l2 = manage_label();
        int l3 = manage_label();
        Operand label_start = createOperand_INT(OP_LABEL, l1, NULL);
        Operand label_loop = createOperand_INT(OP_LABEL, l2, NULL);
        Operand label_end = createOperand_INT(OP_LABEL, l3, NULL);
        InterCodes codes_expr = translate_Condition(expr, label_loop, label_end, tables, depth);
        InterCodes codes_stmt = translate_Stmt(stmt, tables, depth);
        InterCodes codes_start = initNewInterCodes();

        InterCode code_label_start = createInterCode_UNARY(label_start, I_LABEL);
        InterCode code_label_loop = createInterCode_UNARY(label_loop, I_LABEL);
        InterCode code_label_end = createInterCode_UNARY(label_end, I_LABEL);
        InterCode code_goto_start = createInterCode_UNARY(label_start, I_GOTO);

        addInterCode(codes_start, code_label_start);
        addInterCodesAsChild(codes, codes_start);
        
        addInterCode(codes_expr, code_label_loop);
        addInterCode(codes_stmt, code_goto_start);
        addInterCode(codes_stmt, code_label_end);
        
        addInterCodesAsChild(codes, codes_expr);
        addInterCodesAsChild(codes, codes_stmt);

        
    }
    return codes;
}



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
    InterCodes codes = initNewInterCodes();
    assert(block != NULL);
    T* vardefstmt_list = block->child->r_brother;
    //跳过局部变量定义的翻译
    T* sentence_list = vardefstmt_list->r_brother;
    assert(sentence_list != NULL);
    T* sentence = sentence_list->child;
    
    while (sentence != NULL)
    {
        T* stmt = sentence->child;
        assert(stmt != NULL);
        InterCodes newcodes = translate_Stmt(stmt, tables, depth);
        //printInterCodes(newcodes);
        addInterCodesAsChild(codes, newcodes);

        //将sentence指向下一条sentence
        sentence = sentence->r_brother;
        if(sentence != NULL)
            sentence = sentence->child;
    }
    
    return codes;
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

InterCodes translate_Program(T* program)
{
    assert(program != NULL);
    RBRoot* tables[MAX_DEPTH];
    int depth = -1;
    tables[++depth] = program->table;
    T* deflist = program->child;
    InterCodes codes = initNewInterCodes();
    while(deflist != NULL && deflist->child != NULL)
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
            InterCodes codes_func_def = initNewInterCodes();
            InterCode function = createInterCode_FUNDEF(fun_id->id, return_type->child->type_no);
            addInterCode(codes_func_def, function);
            addInterCodesAsChild(codes, codes_func_def);
            //翻译参数, 如果lp的右边不是rp
            T* lp = fun_id->r_brother;
            if(lp->r_brother->type_no != RP)
            {
                InterCodes codes_params = translate_args(lp, I_PARAM, tables, depth);
                addInterCodesAsChild(codes, codes_params);
            }
            //翻译block
            T* block = fun_dec->r_brother;
            tables[++depth] = block->table;
            InterCodes codes_func_block = translate_block(block, tables, depth);
            depth--;
            addInterCodesAsChild(codes, codes_func_block);
            
        }
        deflist = child->r_brother;
    }

    return codes;
}
