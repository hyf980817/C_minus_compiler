#include <stdio.h>
#include <string.h>
#include "InterCode.h"


int test1()
{
    char *name = "a";
    Operand i1 = createOperand_INT(OP_CONST, 3, NULL);
    Operand i2 = createOperand_INT(OP_VAR, 4, name);
    Operand i3 = createOperand_INT(OP_TEMP, 1, NULL);
    Operand i4 = createOperand_INT(OP_LABEL, 1, NULL);

    InterCode c1 = createInterCode_ASSIGN(i1, i3);
    InterCode c2 = createInterCode_BINOP(i2, i1, i3, I_ADD);
    InterCode c3 = createInterCode_UNARY(i4, I_GOTO);
    printInterCode(c1);
    printInterCode(c2);
    printInterCode(c3);
}




int main(void)
{
    test1();

    return 0;
}