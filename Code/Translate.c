#include "InterCode.h"
#include "Tree.h"

InterCodes translate_Exp(T* Exp, RBRoot* tables[], int depth, Operand place)
{
    InterCodes codes = initNewInterCodes();
    T* child = Exp->child;
    if(child->type[0] == 'I')   //Exp->ID或Exp->INT
    {
        Operand op = NULL;
        if(child->type[1] == 'N')  //Exp->INT, int常量
        {
            op = createOperand_INT(OP_CONST, child->int_val,NULL);
        }
        if(child->type[1] == 'D')
        {
            op = createOperand_INT(OP_VAR, 0, child->id);
        }
    }
}
