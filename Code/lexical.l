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
 /*空白符*/
(" "|"\t")+     {;}
 /*注释*/
"/*"(.)*"*/"  {;}
\n            {yycolumn = 1;}
    /*类型*/
int             {yylval = initTreeNode("TYPE_INT", TYPE_INT); yylval->lineno = yylineno; return TYPE_INT;}
char            {yylval = initTreeNode("TYPE_CHAR", TYPE_CHAR); yylval->lineno = yylineno; return TYPE_CHAR;}
float           {yylval = initTreeNode("TYPE_FLOAT",TYPE_FLOAT); yylval->lineno = yylineno; return TYPE_FLOAT;}
void            {yylval = initTreeNode("VOID", VOID); yylval->lineno = yylineno; return VOID;}

    /*关键字*/
return          {yylval = initTreeNode("RETURN",RETURN); yylval->lineno = yylineno; return RETURN;}
if              {yylval = initTreeNode("IF", IF); yylval->lineno = yylineno; return IF;}
else            {yylval = initTreeNode("ELSE", ELSE); yylval->lineno = yylineno; return ELSE;}
while           {yylval = initTreeNode("WHILE", WHILE); yylval->lineno = yylineno; return WHILE;}
for             {yylval = initTreeNode("FOR", FOR); yylval->lineno = yylineno; return FOR;}
break           {yylval = initTreeNode("BREAK", BREAK); yylval->lineno = yylineno; return BREAK;}
continue        {yylval = initTreeNode("CONTINUE", CONTINUE); yylval->lineno = yylineno; return CONTINUE;}

    /*标识符*/
{letter}({digit}|{letter})*     {yylval = initTreeNode("ID", ID); yylval->id = strdup(yytext); yylval->lineno = yylineno; return ID;}

    /*整型*/
{non_zero}{digits}|{digit}  {yylval = initTreeNode("INT", INT); yylval->int_val = atoi(yytext); yylval->lineno = yylineno; return INT;}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {yylval = initTreeNode("FLOAT", FLOAT); yylval->lineno = yylineno; return FLOAT;}

    /*字符*/
\'[a-zA-Z]\'        {yylval = initTreeNode("CHAR", CHAR); yylval->lineno = yylineno; return CHAR;}

    /*字符串*/
\"[^\n]\"           {yylval = initTreeNode("STRING", STRING);return STRING;}
    /*符号*/
"."             {yylval = initTreeNode("POINT", POINT); yylval->lineno = yylineno; return POINT;}
";"             {yylval = initTreeNode("SEMI", SEMI); yylval->lineno = yylineno; return SEMI;}
","             {yylval = initTreeNode("COMMA", COMMA); yylval->lineno = yylineno; return COMMA;}
"'"             {yylval = initTreeNode("S_QUOTA", S_QUOTA); yylval->lineno = yylineno; return S_QUOTA;}
\"              {yylval = initTreeNode("QUOTA", QUOTA); yylval->lineno = yylineno; return QUOTA;}

"("             {yylval = initTreeNode("LP", LP); yylval->lineno = yylineno; return LP;}    
")"             {yylval = initTreeNode("RP", RP); yylval->lineno = yylineno; return RP;}    
"["             {yylval = initTreeNode("LB", LB); yylval->lineno = yylineno; return LB;}    
"]"             {yylval = initTreeNode("RB", RB); yylval->lineno = yylineno; return RB;}    
"{"             {yylval = initTreeNode("LC", LC); yylval->lineno = yylineno; return LC;}    
"}"             {yylval = initTreeNode("RC", RC); yylval->lineno = yylineno; return RC;}    


"*"              {yylval = initTreeNode("OP_STAR", OP_STAR); yylval->lineno = yylineno; return OP_STAR;}    
"-"              {yylval = initTreeNode("OP_SUB", OP_SUB); yylval->lineno = yylineno; return OP_SUB;}    
"/"              {yylval = initTreeNode("OP_DIV", OP_DIV); yylval->lineno = yylineno; return OP_DIV;}    
"%"              {yylval = initTreeNode("OP_MOD", OP_MOD); yylval->lineno = yylineno; return OP_MOD;}    
"+"              {yylval = initTreeNode("OP_ADD", OP_ADD); yylval->lineno = yylineno; return OP_ADD;}    
">>"              {yylval = initTreeNode("OP_SHR", OP_SHR); yylval->lineno = yylineno; return OP_SHR;}    
"<<"              {yylval = initTreeNode("OP_SHL", OP_SHL); yylval->lineno = yylineno; return OP_SHL;}    
">"              {yylval = initTreeNode("OP_GT", OP_GT); yylval->lineno = yylineno; return OP_GT;}    
"<"              {yylval = initTreeNode("OP_LT", OP_LT); yylval->lineno = yylineno; return OP_LT;}    
">="              {yylval = initTreeNode("OP_GE", OP_GE); yylval->lineno = yylineno; return OP_GE;}    
"<="              {yylval = initTreeNode("OP_LE", OP_LE); yylval->lineno = yylineno; return OP_LE;}    
"=="              {yylval = initTreeNode("OP_EQ", OP_EQ); yylval->lineno = yylineno; return OP_EQ;}    
"!="              {yylval = initTreeNode("OP_NEQ", OP_NEQ); yylval->lineno = yylineno; return OP_NEQ;}    
"&"              {yylval = initTreeNode("OP_BIT_AND", OP_BIT_AND); yylval->lineno = yylineno; return OP_BIT_AND;}
"^"              {yylval = initTreeNode("OP_BIT_XOR", OP_BIT_XOR); yylval->lineno = yylineno; return OP_BIT_XOR;}    
"|"              {yylval = initTreeNode("OP_BIT_OR", OP_BIT_OR); yylval->lineno = yylineno; return OP_BIT_OR;}    
"&&"              {yylval = initTreeNode("OP_AND", OP_AND); yylval->lineno = yylineno; return OP_AND;}    
"||"              {yylval = initTreeNode("OP_OR", OP_OR); yylval->lineno = yylineno; return OP_OR;}    
"="              {yylval = initTreeNode("OP_ASSIGN", OP_ASSIGN); yylval->lineno = yylineno; return OP_ASSIGN;}    



.           {fprintf(stderr, "Error at line %d column %d, unknown token:%c\n", yylineno, yycolumn,yytext[0]);}


%%

