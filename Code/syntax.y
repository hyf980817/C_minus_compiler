%locations

%{
    /*这一部分只能写定义, 不能写陈述语句, 比如a=3这种语句是不允许的*/
    #include <stdio.h>
    #include <stdarg.h> 
    #include <string.h>  
    #include <assert.h>
    #include "lex.yy.c"
    T* TreeRoot = NULL; 
    
    int ERROR = 0;
    int yyerror(const char* msg);
    int yylex(void);
%}

%define api.value.type {struct T*}
%define parse.error verbose

%token TYPE_INT TYPE_CHAR TYPE_FLOAT

%token INT
%token FLOAT
%token CHAR
%token ID STRING
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
Program : DefList  {$$ = initTreeNode(yytname[yyr1[yyn]], Program); insertChild($$, 1, $1); TreeRoot = $$;}   
    ;

 /*DefList: 定义声明串, 由0个或多个DefDec(定义声明)组成*/
DefList :  /*empty*/ {$$ = initTreeNode(yytname[yyr1[yyn]],DefList); }
    | VarDefStmt DefList {$$ = initTreeNode(yytname[yyr1[yyn]], DefList); insertChild($$, 2, $1, $2);}
    | FunDef DefList {$$ = initTreeNode(yytname[yyr1[yyn]], DefList); insertChild($$, 2, $1, $2);}
    ;



 /*Def: 变量定义语句, 如int a,b;*/
VarDefStmt : TYPE VarDecList SEMI   {$$ = initTreeNode(yytname[yyr1[yyn]], VarDefStmt); insertChild($$, 3, $1, $2, $3);}
    | error SEMI {$$ = initTreeNode(yytname[yyr1[yyn]], VarDefStmt);}
    | error {$$ = initTreeNode(yytname[yyr1[yyn]], VarDefStmt);}
    ;

 /*FunDef:函数定义*/
FunDef : TYPE FunDec BLOCK     {$$ = initTreeNode(yytname[yyr1[yyn]], FunDef); insertChild($$, 3, $1, $2, $3);}
    ;


 /*VarDecList: 变量声明串, 由一个或者由逗号分隔开的多个VarDec(变量名)组成*/
VarDecList : VarDec     {$$ = initTreeNode(yytname[yyr1[yyn]], VarDecList); insertChild($$, 1, $1);}
    | VarDec COMMA VarDecList     {$$ = initTreeNode(yytname[yyr1[yyn]], VarDecList); insertChild($$, 3, $1, $2, $3);}
    ;
 /*TYPE: 类型标识符*/
TYPE : TYPE_INT      {$$ = initTreeNode(yytname[yyr1[yyn]], TYPE); insertChild($$, 1, $1);} 
    | TYPE_FLOAT     {$$ = initTreeNode(yytname[yyr1[yyn]], TYPE); insertChild($$, 1, $1);}
    | TYPE_CHAR      {$$ = initTreeNode(yytname[yyr1[yyn]], TYPE); insertChild($$, 1, $1);}   
    ;
 /*VarDec: 对一个变量的定义,标识符或者数组, 如int a中的a, int a[3]中的a[3]*/
VarDec : ID     {$$ = initTreeNode(yytname[yyr1[yyn]], VarDec); insertChild($$, 1, $1);}        
    | ID OP_ASSIGN Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], VarDec); insertChild($$, 3, $1, $2, $3);}
    | ID LB INT RB     {$$ = initTreeNode(yytname[yyr1[yyn]], VarDec); insertChild($$, 4, $1, $2, $3, $4);}
    ;

 /*FunDec:函数声明, fun(), fun(int a, int b).*/
FunDec : ID LP ParaList RP     {$$ = initTreeNode(yytname[yyr1[yyn]], FunDec); insertChild($$, 4, $1, $2, $3, $4);}
    | ID LP RP     {$$ = initTreeNode(yytname[yyr1[yyn]], FunDec); insertChild($$, 3, $1, $2, $3);}
    ;
 /*ParaList : 函数参数列表, 上面例子中的int a, int b即是*/
ParaList : ParaDec COMMA ParaList     {$$ = initTreeNode(yytname[yyr1[yyn]], ParaList); insertChild($$, 3, $1, $2, $3);}
    | ParaDec     {$$ = initTreeNode(yytname[yyr1[yyn]], ParaList); insertChild($$, 1, $1);}
    ;

 /*ParaDec: 参数定义*/
ParaDec : TYPE VarDec      {$$ = initTreeNode(yytname[yyr1[yyn]], ParaDec); insertChild($$, 2, $1, $2);}
    ;

 /*Block : 由花括号括起来的一堆语句*/
BLOCK : LC VarDefStmtList SentenceList RC     {$$ = initTreeNode(yytname[yyr1[yyn]], BLOCK); insertChild($$, 4, $1, $2, $3, $4);}
    ;

 /*一串定义变量的语句*/
VarDefStmtList : /*empty*/ {$$ = initTreeNode(yytname[yyr1[yyn]], VarDefStmtList); }
    | VarDefStmt VarDefStmtList {$$ = initTreeNode(yytname[yyr1[yyn]], VarDefStmtList); insertChild($$, 2, $1, $2);}


 /* StmtList: 语句串*/
SentenceList : Sentence SentenceList     {$$ = initTreeNode(yytname[yyr1[yyn]], SentenceList); insertChild($$, 2, $1, $2);}
    | /*empty*/{$$ = initTreeNode(yytname[yyr1[yyn]], SentenceList); }
    ;


 /*Stmt: 语句, 分为定义语句和陈述语句*/
Sentence : Stmt     {$$ = initTreeNode(yytname[yyr1[yyn]], Sentence); insertChild($$, 1, $1);}
    | error SEMI {$$ = initTreeNode(yytname[yyr1[yyn]], Sentence);}
    | error {$$ = initTreeNode(yytname[yyr1[yyn]], Sentence);}
    ;


 /*陈述语句*/
Stmt : Expr SEMI     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 2, $1, $2);}
    | BLOCK     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 1, $1);}
    | RETURN Expr SEMI     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 3, $1, $2, $3);}
    | IF LP Expr RP Stmt  %prec LOWER_THAN_ELSE     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 5, $1, $2, $3, $4, $5);}
    | IF LP Expr RP Stmt ELSE Stmt     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Expr RP Stmt     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 5, $1, $2, $3, $4, $5);}
    | FOR LP Expr SEMI Expr SEMI Expr RP Stmt     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 9, $1, $2, $3, $4, $5, $6, $7, $8, $9);}
    | BREAK SEMI     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 2, $1, $2);}
    | CONTINUE SEMI     {$$ = initTreeNode(yytname[yyr1[yyn]], Stmt); insertChild($$, 2, $1, $2);}
    ;


 /*表达式*/
Expr : Expr OP_ASSIGN Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_OR Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_XOR Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_BIT_AND Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_EQ Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_NEQ Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_GT Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_LT Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_GE Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_LE Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SHL Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SHR Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_ADD Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_SUB Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_STAR Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_DIV Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr OP_MOD Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | LP Expr RP     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | ID     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 1, $1);}
    | FLOAT     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 1, $1);}
    | INT     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 1, $1);}      
    | CHAR     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 1, $1);}
    | ID LP Args RP     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 4, $1, $2, $3, $4);}
    | ID LP RP     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 3, $1, $2, $3);}
    | Expr LB Expr RB     {$$ = initTreeNode(yytname[yyr1[yyn]], Expr); insertChild($$, 4, $1, $2, $3, $4);}
    ;



Args : Expr COMMA Args     {$$ = initTreeNode(yytname[yyr1[yyn]], Args); insertChild($$, 3, $1, $2, $3);}
    | Expr     {$$ = initTreeNode(yytname[yyr1[yyn]], Args); insertChild($$, 1, $1);}
    ;


%%
int yyerror(const char* msg)
{
    fprintf(stderr, "Error at line %d: %s\n", yylineno, msg);
    ERROR = 1;
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

void printTree(T* root, int level, FILE *f)
{
    for(int i = 0; i < level; i++)
        fprintf(f,"    ");
    if(strcmp(root->type, "ID") == 0)
        fprintf(f,"%s(%s)\n", root->type, root->id);
    else
        fprintf(f,"%s\n", root->type);

    if(strcmp(root->type, "Program") == 0 || strcmp(root->type, "BLOCK") == 0)
    {
        printf("\n%s SymbolTable:\n", root->type);
        print_rbtree(root->table);
        printf("\nend of SymbolTable:\n");
    }
    T* child = root->child;
    while(child != NULL)
    {
        printTree(child, level + 1, f);
        child = child->r_brother;
        
    }
}


int main(int argc, char** argv)
{
    //freopen("out.txt", "w",stdout);
    //freopen("err.txt", "w",stderr);
    FILE* f = NULL;
    if(argc <= 1) 
        f = fopen("../Test/test1.cmm", "r");
    f = fopen(argv[1], "r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
    //yydebug = 1;
    yyrestart(f);
    yyparse();
    FILE* f1 = fopen("parser.tree", "w");
    if(!ERROR)
    {
        addSymbolTable(TreeRoot);
        printTree(TreeRoot, 0, f1);
    }

    return 0;
}