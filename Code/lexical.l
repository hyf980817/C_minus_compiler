%{
    #include <stdio.h>
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
int             {return TYPE_INT;}
char            {return TYPE_CHAR;}
float           {return TYPE_FLOAT;}
void            {return VOID;}

    /*关键字*/
return          {return RETURN;}
if              {return IF;}
else            {return ELSE;}
while           {return WHILE;}
for             {return FOR;}
break           {return BREAK;}
continue        {return CONTINUE;}

    /*标识符*/
{letter}({digit}|{letter})*     {yylval.type_str = strdup(yytext); return ID;}

    /*整型*/
{non_zero}{digits}|{digit}  {yylval.type_int = atoi(yytext); return INT;}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {yylval.type_str = strdup(yytext); return FLOAT;}

    /*字符*/
\'[a-zA-Z]\'        {yylval.type_str = strdup(yytext); return CHAR;}

    /*字符串*/
\"[^\n]\"           {yylval.type_str = strdup(yytext); return STRING;}
    /*符号*/
"."             {return POINT;}
";"             {return SEMI;}
","             {return COMMA;}
"'"             {return S_QUOTA;}
\"              {return QUOTA;}

"("             {return LP;}    
")"             {return RP;}    
"["             {return LB;}    
"]"             {return RB;}    
"{"             {return LC;}    
"}"             {return RC;}    


"*"              {return OP_STAR;}    
"-"              {return OP_SUB;}    
"/"              {return OP_DIV;}    
"%"              {return OP_MOD;}    
"+"              {return OP_ADD;}    
">>"              {return OP_SHR;}    
"<<"              {return OP_SHL;}    
">"              {return OP_GT;}    
"<"              {return OP_LT;}    
">="              {return OP_GE;}    
"<="              {return OP_LE;}    
"=="              {return OP_EQ;}    
"!="              {return OP_NEQ;}    
"&"              {return OP_BIT_AND;}
"^"              {return OP_BIT_XOR;}    
"|"              {return OP_BIT_OR;}    
"="              {return OP_ASSIGN;}    



.           {printf("Unknown at line %d:%c", yylineno, yytext[0]);}


%%
