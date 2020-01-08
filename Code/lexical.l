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
int             {yylval = initTreeNode("TYPE_INT", TYPE_INT); return TYPE_INT;}
char            {yylval = initTreeNode("TYPE_CHAR", TYPE_CHAR); return TYPE_CHAR;}
float           {yylval = initTreeNode("TYPE_FLOAT",TYPE_FLOAT); return TYPE_FLOAT;}
void            {yylval = initTreeNode("VOID", VOID); return VOID;}

    /*关键字*/
return          {yylval = initTreeNode("RETURN",RETURN); return RETURN;}
if              {yylval = initTreeNode("IF", IF); return IF;}
else            {yylval = initTreeNode("ELSE", ELSE); return ELSE;}
while           {yylval = initTreeNode("WHILE", WHILE); return WHILE;}
for             {yylval = initTreeNode("FOR", FOR); return FOR;}
break           {yylval = initTreeNode("BREAK", BREAK); return BREAK;}
continue        {yylval = initTreeNode("CONTINUE", CONTINUE); return CONTINUE;}

    /*标识符*/
{letter}({digit}|{letter})*     {yylval = initTreeNode("ID", ID); yylval->id = strdup(yytext); return ID;}

    /*整型*/
{non_zero}{digits}|{digit}  {yylval = initTreeNode("INT", INT); yylval->int_val = atoi(yytext); return INT;}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {yylval = initTreeNode("FLOAT", FLOAT); return FLOAT;}

    /*字符*/
\'[a-zA-Z]\'        {yylval = initTreeNode("CHAR", CHAR); return CHAR;}

    /*字符串*/
\"[^\n]\"           {yylval = initTreeNode("STRING", STRING);return STRING;}
    /*符号*/
"."             {yylval = initTreeNode("POINT", POINT); return POINT;}
";"             {yylval = initTreeNode("SEMI", SEMI); return SEMI;}
","             {yylval = initTreeNode("COMMA", COMMA); return COMMA;}
"'"             {yylval = initTreeNode("S_QUOTA", S_QUOTA); return S_QUOTA;}
\"              {yylval = initTreeNode("QUOTA", QUOTA); return QUOTA;}

"("             {yylval = initTreeNode("LP", LP); return LP;}    
")"             {yylval = initTreeNode("RP", RP); return RP;}    
"["             {yylval = initTreeNode("LB", LB); return LB;}    
"]"             {yylval = initTreeNode("RB", RB); return RB;}    
"{"             {yylval = initTreeNode("LC", LC); return LC;}    
"}"             {yylval = initTreeNode("RC", RC); return RC;}    


"*"              {yylval = initTreeNode("OP_STAR", OP_STAR); return OP_STAR;}    
"-"              {yylval = initTreeNode("OP_SUB", OP_SUB); return OP_SUB;}    
"/"              {yylval = initTreeNode("OP_DIV", OP_DIV); return OP_DIV;}    
"%"              {yylval = initTreeNode("OP_MOD", OP_MOD); return OP_MOD;}    
"+"              {yylval = initTreeNode("OP_ADD", OP_ADD); return OP_ADD;}    
">>"              {yylval = initTreeNode("OP_SHR", OP_SHR); return OP_SHR;}    
"<<"              {yylval = initTreeNode("OP_SHL", OP_SHL); return OP_SHL;}    
">"              {yylval = initTreeNode("OP_GT", OP_GT); return OP_GT;}    
"<"              {yylval = initTreeNode("OP_LT", OP_LT); return OP_LT;}    
">="              {yylval = initTreeNode("OP_GE", OP_GE); return OP_GE;}    
"<="              {yylval = initTreeNode("OP_LE", OP_LE); return OP_LE;}    
"=="              {yylval = initTreeNode("OP_EQ", OP_EQ); return OP_EQ;}    
"!="              {yylval = initTreeNode("OP_NEQ", OP_NEQ); return OP_NEQ;}    
"&"              {yylval = initTreeNode("OP_BIT_AND", OP_BIT_AND); return OP_BIT_AND;}
"^"              {yylval = initTreeNode("OP_BIT_XOR", OP_BIT_XOR); return OP_BIT_XOR;}    
"|"              {yylval = initTreeNode("OP_BIT_OR", OP_BIT_OR); return OP_BIT_OR;}    
"&&"              {yylval = initTreeNode("OP_AND", OP_AND); return OP_AND;}    
"||"              {yylval = initTreeNode("OP_OR", OP_OR); return OP_OR;}    
"="              {yylval = initTreeNode("OP_ASSIGN", OP_ASSIGN); return OP_ASSIGN;}    



.           {fprintf(stderr, "Error at line %d column %d, unknown token:%c\n", yylineno, yycolumn,yytext[0]);}


%%

