%{
    #include <stdio.h>
    #include "syntax.tab.h"
    #include <stdarg.h>
    #include "Tree.h"
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
"/*"(.)*"*/"  {;}
\n            {fprintf(stderr, "get a line change"); yycolumn = 1;}
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
{letter}({digit}|{letter})*     {yylval = initTreeNode("ID"); return ID;}

    /*整型*/
{non_zero}{digits}|{digit}  {yylval = initTreeNode("INT"); return INT;}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {yylval = initTreeNode("FLOAT"); return FLOAT;}

    /*字符*/
\'[a-zA-Z]\'        {yylval = initTreeNode("CHAR"); return CHAR;}

    /*字符串*/
\"[^\n]\"           {yylval = initTreeNode("STRING");return STRING;}
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



.           {fprintf(stdout, "Lexical Error at line %d:%c", yylineno, yytext[0]);}


%%

