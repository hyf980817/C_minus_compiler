
#include "Tree.h"
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

//将c插入为root的child节点
void insertChild(T* root, T* newnode)
{
    root->child = newnode;
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

/*更新语法树
  两种情况:
  1. 压入新的符号, isReduction为false, 这时只需要提供新的节点, 将其插入为root的最右端brother
  2. 进行规约, isReduction为true. 此时, root最右侧的reduceLength个节点需要按顺序成为新插入的newnode的child.
*/
T* updateSyntaxTree(T* root, char * name,int isReduction, int reduceLength)
{
    T* newnode = initTreeNode(name);
    if(root == NULL)
    {
        if(isReduction)
            printf("Error occurs in function: updateSyntaxTree!!!");
        root = newnode;
        return root;
    }
    
    T* end = root;
    
    while(end->r_brother != NULL)
        end = end->r_brother;
    if(isReduction)
    {

        //向前回溯reduceLength - 1步
        int i = 0;
        while(i < reduceLength - 1)
            end = end->l_brother;

        //此时end指向的节点就是newnode的child
        newnode->child = end;
        end->l_brother->r_brother = newnode;
        newnode->l_brother = end->l_brother;
        end->l_brother = NULL;
    }
    else
    {
        insertBrotherToRight(root, newnode);
    }
    return root;

}
