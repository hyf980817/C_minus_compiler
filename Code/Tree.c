#include <stdio.h>
#include "Tree.h"
#include <stdarg.h>   


//初始化一个节点, 目前我们只需要节点信息包括(非)终结符号的名字
T* initTreeNode(char* name)
{
    T* result = (T*)malloc(sizeof(T));
    result->child = NULL;
    result->l_brother = NULL;
    result->r_brother = NULL;
    result->name = strdup(name);

    return result;
}

//将一个或多个节点插入为root的child节点
void insertChild(T* root, int n, ...)
{
    int i = 0;
    va_list ap;
    va_start(ap, n);
    
    T* child = va_arg(ap, T*);

    root->child = child;
    for(i = 1; i < n; i++)
    {
        child = va_arg(ap, T*);
        insertBrotherToRight(root->child, child);
    }

    va_end(ap);       
}

//将c插入作为root的brother节点(如果已经有brother节点,则插入到最右边)
void insertBrotherToRight(T* root, T* newnode)
{
    T* brother_end = root;
    while(brother_end -> r_brother != NULL)
        brother_end = brother_end -> r_brother;
    brother_end->r_brother = newnode;
    newnode->l_brother = brother_end;
}

