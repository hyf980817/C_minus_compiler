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

%define api.value.type {struct T*}


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

%type <T*> Program DefDecList DefDec VarDefStmt VarDecList TYPE FunDec VarDec 
%type <T*> Expr ParaList ParaDec Args BLOCK Sentence SentenceList Stmt StmtList 

 /*消除IF-ELSE二义性*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%
 /*Program:开始符号, 整个程序*/
Program : DefDecList  {insertChild($$, 1, $1); TreeRoot = $$;}   
    ;

 /*DefDecList: 定义声明串, 由0个或多个DefDec(定义声明)组成*/
DefDecList :  /*empty*/
    | DefDec DefDecList {insertChild($$, 2, $1, $2);}
    ;

 /*Def: 变量定义语句, 如int a,b;*/
VarDefStmt : TYPE VarDecList SEMI   {insertChild($$, 3, $1, $2, $3);}
    ;

 /*DefDec:定义声明, 表示一个全局变量或者函数的定义/声明, 两个产生式, 前者是全局变量, 后者是函数*/
DefDec : VarDefStmt     {insertChild($$, 1, $1);}
    | TYPE FunDec BLOCK     {insertChild($$, 3, $1, $2, $3);}
    ;
 /*VarDecList: 变量声明串, 由一个或者由逗号分隔开的多个VarDec(变量名)组成*/
VarDecList : VarDec     {insertChild($$, 1, $1);}
    | VarDec COMMA VarDecList     {insertChild($$, 3, $1, $2, $3);}
    ;
 /*TYPE: 类型标识符*/
TYPE : TYPE_INT      {insertChild($$, 1, $1);} 
    | TYPE_FLOAT     {insertChild($$, 1, $1);}
    | TYPE_CHAR      {insertChild($$, 1, $1);}   
    ;
 /*VarDec: 对一个变量的定义,标识符或者数组, 如int a中的a, int a[3]中的a[3]*/
VarDec : ID     {insertChild($$, 1, $1);}        
    | ID OP_ASSIGN Expr     {insertChild($$, 4, $1, $2, $3, $4);}
    | VarDec LB INT RB     {insertChild($$, 4, $1, $2, $3, $4);}
    ;

 /*FunDec:函数声明, fun(), fun(int a, int b).*/
FunDec : ID LP ParaList RP     {insertChild($$, 4, $1, $2, $3, $4);}
    | ID LP RP     {insertChild($$, 3, $1, $2, $3);}
    ;
 /*ParaList : 函数参数列表, 上面例子中的int a, int b即是*/
ParaList : ParaDec COMMA ParaList     {insertChild($$, 3, $1, $2, $3);}
    | ParaDec     {insertChild($$, 1, $1);}
    ;

 /*ParaDec: 参数定义*/
ParaDec : TYPE VarDec      {insertChild($$, 2, $1, $2);}
    ;

 /*Block : 由花括号括起来的一堆语句*/
BLOCK : LC SentenceList RC     {insertChild($$, 3, $1, $2, $3);}
    ;

 /* StmtList: 语句串*/
SentenceList : Sentence SentenceList     {insertChild($$, 2, $1, $2);}
    | /*empty*/
    ;
 /*Stmt: 语句, 分为定义语句和陈述语句*/
Sentence : VarDefStmt     {insertChild($$, 1, $1);}
    | Stmt     {insertChild($$, 1, $1);}
    ;


 /*陈述语句*/
Stmt : Expr SEMI     {insertChild($$, 2, $1, $2);}
    | BLOCK     {insertChild($$, 1, $1);}
    | RETURN Expr SEMI     {insertChild($$, 3, $1, $2, $3);}
    | IF LP Expr RP Stmt  %prec LOWER_THAN_ELSE     {insertChild($$, 5, $1, $2, $3, $4, $5);}
    | IF LP Expr RP Stmt ELSE Stmt     {insertChild($$, 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Expr RP Stmt     {insertChild($$, 5, $1, $2, $3, $4, $5);}
    | FOR LP Expr SEMI Expr SEMI Expr SEMI RP Stmt     {insertChild($$, 10, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10);}
    | BREAK SEMI     {insertChild($$, 2, $1, $2);}
    | CONTINUE SEMI     {insertChild($$, 2, $1, $2);}
    ;

Expr : Expr OP_ASSIGN Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_OR Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_XOR Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_AND Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_EQ Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_NEQ Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_GT Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_LT Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_GE Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_LE Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SHL Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SHR Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_ADD Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SUB Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_STAR Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_DIV Expr     {insertChild($$, 3, $1, $2, $3);}
    | Expr OP_MOD Expr     {insertChild($$, 3, $1, $2, $3);}
    | LP Expr RP     {insertChild($$, 3, $1, $2, $3);}
    | ID     {insertChild($$, 1, $1);}
    | FLOAT     {insertChild($$, 1, $1);}
    | INT     {insertChild($$, 1, $1);}      
    | CHAR     {insertChild($$, 1, $1);}
    | ID LP Args RP     {insertChild($$, 4, $1, $2, $3, $4);}
    | ID LP RP     {insertChild($$, 3, $1, $2, $3);}
    | Expr LB Expr RB     {insertChild($$, 4, $1, $2, $3, $4);}
    ;

Args : Expr COMMA Args     {insertChild($$, 3, $1, $2, $3);}
    | Expr     {insertChild($$, 1, $1);}
    ;
%%
int
yyerror(char* msg)
{
    fprintf(stderr, "error: %s\n", msg);
    return 0;
}

