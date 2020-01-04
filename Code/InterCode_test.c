#include <stdio.h>
#include <string.h>
#include "InterCode.h"
#include "Tree.h"
#include <stdarg.h>
#include "syntax.tab.h"
#include <assert.h>
#include <stdlib.h>
#include "Translate.c"

void printTree(T* root, int level)
{

    for(int i = 0; i < level; i++)
        printf("    ");
    if(strcmp(root->type, "ID") == 0)
        printf("%s(%s)\n", root->type, root->id);
    else
        printf("%s\n", root->type);

    if(strcmp(root->type, "Program") == 0 || strcmp(root->type, "BLOCK") == 0)
    {
        printf("\n%s SymbolTable:\n", root->type);
        print_rbtree(root->table);
        printf("\nend of SymbolTable:\n");
    }
    T* child = root->child;
    while(child != NULL)
    {
        printTree(child, level + 1);
        child = child->r_brother;
        
    }
}

//测试Intercode
int test1()
{
    char *name = "a";
    Operand i1 = createOperand_INT(OP_CONST, 3, NULL);
    Operand i2 = createOperand_INT(OP_VAR, 4, name);
    Operand i3 = createOperand_INT(OP_TEMP, 1, NULL);
    Operand i4 = createOperand_INT(OP_LABEL, 1, NULL);

    InterCode c1 = createInterCode_ASSIGN(i1, i3);
    InterCode c2 = createInterCode_BINOP(i2, i1, i3, I_BINOP, OP_ADD);
    InterCode c3 = createInterCode_UNARY(i4, I_GOTO);
    printInterCode(c1);
    printInterCode(c2);
    printInterCode(c3);
    printf("end test1\n");
}

//测试InterCodes,以及translate_Exp的使用
int test2()
{
    T* expr = initTreeNode("Expr", Expr);
    T* expr1 = initTreeNode("Expr", Expr);
    T* expr2 = initTreeNode("Expr", Expr);
    T* expr21 = initTreeNode("Expr", Expr);
    T* expr22 = initTreeNode("Expr", Expr);
    T* id1 = initTreeNode("ID", ID);
    id1->id = strdup("a");

    T* id2 = initTreeNode("ID", ID);
    id2->id = strdup("b");

    T* add = initTreeNode("OP_ADD", OP_ADD);
    T* assign = initTreeNode("OP_ASSIGN", OP_ASSIGN);
    T* num = initTreeNode("INT", INT);
    num->int_val = 3;
    insertChild(expr21, 1, id2);
    insertChild(expr22, 1, num);
    insertChild(expr2, 3, expr21, add, expr22);
    insertChild(expr1, 1, id1);
    insertChild(expr, 3, expr1, assign, expr2);

    printTree(expr, 0);
    Operand t1 = createOperand_INT(OP_TEMP, 5, NULL);
    InterCodes codes = translate_Exp(expr, NULL, 0, t1);

    printf("end translate\n");
    PrintInterCodes(codes);
}

int main(void)
{
    test1();
    test2();
    return 0;
}




//初始化一个节点, 目前我们只需要节点信息包括(非)终结符号的名字
T* initTreeNode(const char* const name, int type_no)
{
    T* result = (T*)malloc(sizeof(T));
    result->child = NULL;
    result->l_brother = NULL;
    result->r_brother = NULL;
    result->type_no = type_no;
    result->type = strdup(name);
    result->table = NULL;
    return result;
}

//将一个或多个节点插入为root的child节点
void insertChild(T* root, int n, ...)
{
    int i = 0;
    va_list ap;

    va_start(ap, n);
    
    T* child = va_arg(ap, T*);
    assert(root->child == NULL);
    root->child = child;
  
    //fprintf(stdout, "insert %s in %s as 0 child\n", child->type, root->type);
    for(i = 1; i < n; i++)
    {
        child = va_arg(ap, T*);
        insertBrotherToRight(root->child, child);
        //fprintf(stdout, "insert %s in %s as %d child\n", child->type, root->type, i);
    }

    va_end(ap);       
}

//将c插入作为root的brother节点(如果已经有brother节点,则插入到最右边)
void insertBrotherToRight(T* root, T* newnode)
{
    T* brother_end = root;
    int i = 0;
    while(brother_end -> r_brother != NULL){
        brother_end = brother_end -> r_brother;
        i++;
    }
    brother_end->r_brother = newnode;
    newnode->l_brother = brother_end;
}

