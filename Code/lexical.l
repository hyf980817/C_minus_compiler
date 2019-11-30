%{
    #include <stdio.h>
    #include "Tree.h"
    #include "syntax.tab.h"
   
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}

%option yylineno



letter [_a-zA-Z]
non_zero [1-9]
zero [0]
digit [0-9]
digits [0-9]+


%%

(" "|"\t")+     {;}
\n            {yycolumn = 1;}
    /*类型*/
int             {yylval = initTreeNode("TYPE_INT"); return TYPE_INT;}
char            {yylval = initTreeNode("TYPE_CHAR"); return TYPE_CHAR;}
float           {yylval = initTreeNode("TYPE_FLOAT"); return TYPE_FLOAT;}
void            {yylval = initTreeNode("VOID"); return VOID;}

    /*关键字*/
return          {yylval = initTreeNode("RETURN"); return RETURN;}
if              {yylval = initTreeNode("IF"); return IF;}
else            {yylval = initTreeNode("ELSE"); return ELSE;}
while           {yylval = initTreeNode("WHILE"); return WHILE;}
for             {yylval = initTreeNode("FOR"); return FOR;}
break           {yylval = initTreeNode("BREAK"); return BREAK;}
continue        {yylval = initTreeNode("CONTINUE"); return CONTINUE;}

    /*标识符*/
{letter}({digit}|{letter})*     {yylval = initTreeNode("ID"); yylval.type_str = strdup(yytext); return ID;}

    /*整型*/
{non_zero}{digits}|{digit}  {yylval = initTreeNode("INT"); yylval.type_int = atoi(yytext); return INT;}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {yylval = initTreeNode("FLOAT"); yylval.type_str = strdup(yytext); return FLOAT;}

    /*字符*/
\'[a-zA-Z]\'        {yylval = initTreeNode("CHAR"); yylval.type_char = yytext[0]; return CHAR;}

    /*字符串*/
\"[^\n]\"           {yylval = initTreeNode("STRING"); yylval.type_str = strdup(yytext); return STRING;}
    /*符号*/
"."             {yylval = initTreeNode("POINT"); return POINT;}
";"             {yylval = initTreeNode("SEMI"); return SEMI;}
","             {yylval = initTreeNode("COMMA"); return COMMA;}
"'"             {yylval = initTreeNode("S_QUOTA"); return S_QUOTA;}
\"              {yylval = initTreeNode("QUOTA"); return QUOTA;}

"("             {yylval = initTreeNode("LP"); return LP;}    
")"             {yylval = initTreeNode("RP"); return RP;}    
"["             {yylval = initTreeNode("LB"); return LB;}    
"]"             {yylval = initTreeNode("RB"); return RB;}    
"{"             {yylval = initTreeNode("LC"); return LC;}    
"}"             {yylval = initTreeNode("RC"); return RC;}    


"*"              {yylval = initTreeNode("OP_STAR"); return OP_STAR;}    
"-"              {yylval = initTreeNode("OP_SUB"); return OP_SUB;}    
"/"              {yylval = initTreeNode("OP_DIV"); return OP_DIV;}    
"%"              {yylval = initTreeNode("OP_MOD"); return OP_MOD;}    
"+"              {yylval = initTreeNode("OP_ADD"); return OP_ADD;}    
">>"              {yylval = initTreeNode("OP_SHR"); return OP_SHR;}    
"<<"              {yylval = initTreeNode("OP_SHL"); return OP_SHL;}    
">"              {yylval = initTreeNode("OP_GT"); return OP_GT;}    
"<"              {yylval = initTreeNode("OP_LT"); return OP_LT;}    
">="              {yylval = initTreeNode("OP_GE"); return OP_GE;}    
"<="              {yylval = initTreeNode("OP_LE"); return OP_LE;}    
"=="              {yylval = initTreeNode("OP_EQ"); return OP_EQ;}    
"!="              {yylval = initTreeNode("OP_NEQ"); return OP_NEQ;}    
"&"              {yylval = initTreeNode("OP_BIT_AND"); return OP_BIT_AND;}
"^"              {yylval = initTreeNode("OP_BIT_XOR"); return OP_BIT_XOR;}    
"|"              {yylval = initTreeNode("OP_BIT_OR"); return OP_BIT_OR;}    
"="              {yylval = initTreeNode("OP_ASSIGN"); return OP_ASSIGN;}    



.           {printf("Unknown at line %d:%c", yylineno, yytext[0]);}


%%
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
