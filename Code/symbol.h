#ifndef _SYMBOL_
#define _SYMBOL_
#include "Tree.h"
#define MAX_DEPTH 10
#define VAR_INT 255
#define VAR_FLOAT 256
#define VAR_CHAR 257
#define VAR_VOID 258

typedef struct{
    int id_type;
    union{
        int var_type;
        int return_type;
    };
    int array[MAX_DEPTH]; /*数组类型, 第一个元素存储维度, 如果不是数组, 第一个元素为0,后面的元素是各个维度的长度, 至多支持4维*/
    T* syntaxTreeNode;
}symbol;


/*获取变量的type*/
int getVarType(symbol s);

/*获取函数第n个参数 */
T* getFunVar(symbol s, int n);


#endif

