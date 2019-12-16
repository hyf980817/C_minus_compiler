#ifndef _SYMBOL_
#define _SYMBOL_
#include "Tree.h"
#define FUNC 0
#define VAR 1

typedef struct{
    int type;
    T* syntaxTreeNode;
}symbol;


/*获取变量的type*/
int getVarType(symbol s);

#endif

