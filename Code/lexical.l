%{
    #include <stdio.h>
%}

%option yylineno

letter [_a-zA-Z]
non_zero [1-9]
zero [0]
digit [0-9]
digits [0-9]+


%%

(" "|"\t"|"\n")+    {;}
    /*类型*/
int             {printf("INT_TYPE\n");}
char            {printf("CHAR_TYPE\n");}
float           {printf("FLOAT_TYPE\n");}
void            {printf("VOID_TYPE\n");}

    /*关键字*/
return          {printf("RETURN\n");}
if              {printf("IF\n");}
else            {printf("ELSE\n");}
while           {printf("WHILE\n");}
for             {printf("FOR\n");}

    /*标识符*/
{letter}({digit}|{letter})*     {printf("ID\n");}

    /*整型*/
{non_zero}{digits}|{digit}  {printf("INT_VALUE: %d\n", atoi(yytext));}

    /*浮点*/
({non_zero}{digits}|{digit})"."{digits}|"."{digits}     {printf("FLOAT_VALUE\n");}

    /*字符*/
\'[a-zA-Z]\'        {printf("CHAR_VALUE\n");}


'\n'        {;}
    /*字符串*/
\"[^\n]\"           {printf("STRING\n");}
    /*符号*/
"."             {printf("POINT\n");}
";"             {printf("SEMI\n");}
","             {printf("COMMA\n");}
"'"             {printf("S_QUOTA\n");}
\"              {printf("QUOTA\n");}

"("             {printf("LP\n");}    
")"             {printf("RP\n");}    
"["             {printf("LB\n");}    
"]"             {printf("RB\n");}    
"{"             {printf("LC\n");}    
"}"             {printf("RC\n");}    


"&"              {printf("OP_ADDR\n");}    
"*"              {printf("OP_DEREF or OP_MUL\n");}    
"-"              {printf("OP_SUB or OP_NEG\n");}    
"/"              {printf("OP_DIV\n");}    
"%"              {printf("OP_MOD\n");}    
"+"              {printf("OP_ADD\n");}    
">>"              {printf("OP_SHR\n");}    
"<<"              {printf("OP_SHL\n");}    
">"              {printf("OP_GT\n");}    
"<"              {printf("OP_LT\n");}    
">="              {printf("OP_GE\n");}    
"<="              {printf("OP_LE\n");}    
"=="              {printf("OP_EQ\n");}    
"!="              {printf("OP_NEQ\n");}    
"^"              {printf("OP_XOR\n");}    
"|"              {printf("OP_OR\n");}    
"="              {printf("OP_ASSIGN\n");}    



.           {printf("Misunderstanding at line %d\n", yylineno);}


%%
int main(int argc, char **argv)
{
    if(argc > 1) {
        if(!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
}
