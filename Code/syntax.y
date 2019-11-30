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

%type <T> Program DefDecList DefDec VarDefStmt VarDecList TYPE FunDec VarDec 
%type <T> Expr ParaList ParaDec Args BLOCK Sentence SentenceList Stmt StmtList 

 /*消除IF-ELSE二义性*/
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%
 /*Program:开始符号, 整个程序*/
Program : DefDecList     
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
TYPE : TYPE_INT  
    | TYPE_FLOAT   
    | TYPE_CHAR;    

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
    | INT      
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

