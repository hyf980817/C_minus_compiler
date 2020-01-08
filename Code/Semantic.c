#include "Tree.h"
#include <assert.h>
#include "syntax.tab.h"
#include <stdio.h>
void Block_Analysis(T* block, RBRoot *tables[], int depth);
void Stmt_Analysis(T* stmt, RBRoot *tables[], int depth);


Node* search_tables(char *id, RBRoot *tables[], int depth)
{
    Node *id_node;
    for(int i = depth; i >= 0; i--)
    {
        id_node = search(tables[i]->node, id);
        if(id_node != NULL)
            break;
    }
    return id_node;
}

void Expr_Analysis(T* expr, RBRoot *tables[], int depth)
{
    switch (expr->child->type_no)
    {
    case ID: {//有可能是变量,有可能是函数调用{
        T* id = expr->child;
        Node* id_node = search_tables(id->id, tables, depth);
        if(id_node == NULL)
            printf("Semantic Error:Unknown id in line %d : %s\n", id->lineno, id->id);
        if(id->r_brother != NULL) //函数调用还需要分析参数
        {
            T* lp = id->r_brother;
            //T* fundef_id = (T*)id_node->value.syntaxTreeNode; //调用的函数的定义在语法树上的位置.
            if(lp->r_brother->type_no != RP) //存在参数
            {
                T* args = lp->r_brother;
                while(args != NULL)
                {
                    T* expr1 = args->child;
                    Expr_Analysis(expr1, tables, depth);  //分析每个参数
                    args = args->child->r_brother;
                    if(args != NULL)
                        args = args->r_brother;
                }
            }
            
        }
        break;
    }
    case LP:{ //Expr -> LP Expr RP
        T* expr1 = expr->child->r_brother;
        Expr_Analysis(expr1, tables, depth);
    }
    case Expr:{
        T* expr1 = expr->child;
        T* op = expr1->r_brother;
        assert(op != NULL);
        T* expr2 = op->r_brother;
        assert(expr2 != NULL);
        
        //如果是赋值表达式, 左边必须是ID,或者数组
        if(op->type_no == OP_ASSIGN)
        {
            if(expr1->child->type_no != ID ||(expr1->child->r_brother != NULL && expr1->child->r_brother->type_no != LB))
            {
                T* temp = expr1->child;
                while(expr1->child->type_no == Expr)
                    expr1 = expr1->child;
                printf("Semantic Error in line %d : invalid left value\n", expr1->lineno);
            }
        }

        //然后分别分析两个expr:
        Expr_Analysis(expr1,tables,depth);
        Expr_Analysis(expr2,tables,depth);
        break;
    }
    default:
        break;
    }
}

void Stmt_Analysis(T* stmt, RBRoot *tables[], int depth)
{
    assert(stmt->type_no == Stmt);

    //对不同类型的句子, 进行不同分析
    switch (stmt->child->type_no)
    {
    case Expr:
        Expr_Analysis(stmt->child, tables, depth);
        break;
    case BLOCK:
        tables[++depth] = stmt->child->table;
        Block_Analysis(stmt->child, tables, depth);
        depth--;
        break;
    case RETURN:{
        T* return_expr = stmt->child->r_brother;
        assert(return_expr->type_no == Expr);
        Expr_Analysis(return_expr, tables, depth);
        break;
    }
    case IF:{
        T* expr = stmt->child->r_brother->r_brother;
        assert(expr->type_no == Expr);
        Expr_Analysis(expr, tables, depth);
        T* stmt1 = expr->r_brother->r_brother;
        
        Stmt_Analysis(stmt1, tables, depth);
        if(stmt1->r_brother != NULL)
        {
            T* stmt2 = stmt1->r_brother->r_brother;
            Stmt_Analysis(stmt2, tables, depth);
        }
        break;
    }
    case WHILE:{
        T* expr = stmt->child->r_brother->r_brother;
        assert(expr->type_no == Expr);
        Expr_Analysis(expr, tables, depth);
        T* stmt1 = expr->r_brother->r_brother;
        
        Stmt_Analysis(stmt1, tables, depth);
        break; 
    }      
    default:
        break;
    }
}

//分析一个block,后面两个参数是符号表
void Block_Analysis(T* block, RBRoot *tables[], int depth)
{
    T* sentenceList = block->child->r_brother->r_brother;
    assert(sentenceList->type_no == SentenceList);

    while(sentenceList != NULL)
    {
        T* sentence = sentenceList->child;
        if(sentence != NULL)
        {
            T* stmt = sentence->child;
            Stmt_Analysis(stmt, tables, depth);
            sentenceList = sentence->r_brother; //准备翻译下一条sentence
        }
        else
        {
            sentenceList = NULL;
        }
        
    }
}

/*语义分析*/
void Semantic_Analysis(T* root)
{
    RBRoot* tables[MAX_DEPTH];
    assert(root!=NULL);
    int depth = -1;
    tables[++depth] = root->table; //全局符号表
    RBRoot* global_table = tables[0];
    
    T* deflist = root->child;
    while(deflist !=NULL)
    {
        T* child = deflist->child;
        if(child != NULL && child->type_no == FunDef) //只检查函数里面的语义,全局变量定义已经在创建符号表时搞定了
        {
           T* block = child->child->r_brother->r_brother;
           assert(block->type_no == BLOCK);
           tables[++depth] = block->table;
           Block_Analysis(block, tables, depth);

        }
        deflist = child == NULL ? NULL:child->r_brother;
    }

}

