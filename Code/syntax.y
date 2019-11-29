%locations

%{
    #include <stdio.h>
    #include "lex.yy.c"
    T* TreeRoot = NULL; 
    
%}

/*
%union {
    int type_int;
    float type_float;
    char type_char;
    char* type_str;
}
*/

%define api.value.type {struct T}


%token TYPE_INT TYPE_CHAR TYPE_FLOAT

%token <type_int> INT
%token <type_float> FLOAT
%token <type_char> CHAR
%token <type_str> ID STRING
%token VOID
%token RETURN IF ELSE WHILE FOR BREAK CONTINUE

%token POINT SEMI COMMA S_QUOTA QUOTA
%token LP RP LB RB LC RC

%token OP_STAR OP_DIV OP_MOD OP_ADD OP_SUB OP_SHL OP_SHR OP_GT OP_LT OP_GE OP_LE OP_EQ OP_NEQ OP_BIT_AND OP_BIT_XOR OP_BIT_OR OP_ASSIGN


%right OP_ASSIGN
%left OP_BIT_OR
%left OP_BIT_XOR
%left OP_BIT_AND
%left OP_EQ OP_NEQ
%left OP_GT OP_LT OP_GE OP_LE
%left OP_SHL OP_SHR
%left OP_ADD OP_SUB
%left OP_STAR OP_DIV OP_MOD
%left LP RP LB RB



 /*消除IF-ELSE二义性*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%
 /*Program:开始符号, 整个程序*/
Program : DefDecList  {   
    ;

 /*DefDecList: 定义声明串, 由0个或多个DefDec(定义声明)组成*/
DefDecList :  /*empty*/
    | DefDec DefDecList
    ;

 /*Def: 变量定义语句, 如int a,b;*/
VarDefStmt : TYPE VarDecList SEMI
    ;

 /*DefDec:定义声明, 表示一个全局变量或者函数的定义/声明, 两个产生式, 前者是全局变量, 后者是函数*/
DefDec : VarDefStmt
    | TYPE FunDec BLOCK
    ;
 /*VarDecList: 变量声明串, 由一个或者由逗号分隔开的多个VarDec(变量名)组成*/
VarDecList : VarDec
    | VarDec COMMA VarDecList
    ;
 /*TYPE: 类型标识符*/
TYPE : TYPE_INT | TYPE_FLOAT | TYPE_CHAR;

 /*VarDec: 对一个变量的定义,标识符或者数组, 如int a中的a, int a[3]中的a[3]*/
VarDec : ID
    | ID OP_ASSIGN Expr
    | VarDec LB INT RB
    ;

 /*FunDec:函数声明, fun(), fun(int a, int b).*/
FunDec : ID LP ParaList RP
    | ID LP RP
    ;
 /*ParaList : 函数参数列表, 上面例子中的int a, int b即是*/
ParaList : ParaDec COMMA ParaList
    | ParaDec
    ;

 /*ParaDec: 参数定义*/
ParaDec : TYPE VarDec ;

 /*Block : 由花括号括起来的一堆语句*/
BLOCK : LC SentenceList RC
    ;

 /* StmtList: 语句串*/
SentenceList : Sentence SentenceList
    | /*empty*/
    ;
 /*Stmt: 语句, 分为定义语句和陈述语句*/
Sentence : VarDefStmt
    | Stmt
    ;


 /*陈述语句*/
Stmt : Expr SEMI
    | BLOCK
    | RETURN Expr SEMI
    | IF LP Expr RP Stmt  %prec LOWER_THAN_ELSE
    | IF LP Expr RP Stmt ELSE Stmt
    | WHILE LP Expr RP Stmt
    | FOR LP Expr SEMI Expr SEMI Expr SEMI RP Stmt
    | BREAK SEMI
    | CONTINUE SEMI
    ;

Expr : Expr OP_ASSIGN Expr
    | Expr OP_BIT_OR Expr
    | Expr OP_BIT_XOR Expr
    | Expr OP_BIT_AND Expr
    | Expr OP_EQ Expr
    | Expr OP_NEQ Expr
    | Expr OP_GT Expr
    | Expr OP_LT Expr
    | Expr OP_GE Expr
    | Expr OP_LE Expr
    | Expr OP_SHL Expr
    | Expr OP_SHR Expr
    | Expr OP_ADD Expr
    | Expr OP_SUB Expr
    | Expr OP_STAR Expr
    | Expr OP_DIV Expr
    | Expr OP_MOD Expr
    | LP Expr RP
    | ID
    | FLOAT
    | INT      {printf("INT: %d" ,$1);}
    | CHAR
    | ID LP Args RP
    | ID LP RP
    | Expr LB Expr RB
    ;

Args : Expr COMMA Args
    | Expr
    ;
%%
int
yyerror(char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
    return 0;
}


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

//将c插入作为root的brother节点(插入到root之后, 如果root已经有brother节点, c的brother将指向原本的root的brother)
void insertBrotherToLeft(T* root, T* newnode)
{
    newnode->r_brother = root->r_brother;
    newnode->r_brother->l_brother = newnode;
    root->r_brother = newnode;
    newnode->l_brother = root;
    
}


/*更新语法树
  两种情况:
  1. 压入新的符号, isReduction为false, 这时只需要提供新的节点, 将其插入为root的最右端brother
  2. 进行规约, isReduction为true. 此时, root最右侧的reduceLength个节点需要按顺序成为新插入的newnode的child.
*/
T* updateSyntaxTree(T* root, T* newnode, int isReduction, int reduceLength)
{
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
