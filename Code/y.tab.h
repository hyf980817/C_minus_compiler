/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TYPE_INT = 258,
    TYPE_CHAR = 259,
    TYPE_FLOAT = 260,
    INT = 261,
    FLOAT = 262,
    CHAR = 263,
    ID = 264,
    STRING = 265,
    VOID = 266,
    RETURN = 267,
    IF = 268,
    ELSE = 269,
    WHILE = 270,
    FOR = 271,
    BREAK = 272,
    CONTINUE = 273,
    POINT = 274,
    SEMI = 275,
    COMMA = 276,
    S_QUOTA = 277,
    QUOTA = 278,
    LP = 279,
    RP = 280,
    LB = 281,
    RB = 282,
    LC = 283,
    RC = 284,
    OP_STAR = 285,
    OP_DIV = 286,
    OP_MOD = 287,
    OP_ADD = 288,
    OP_SUB = 289,
    OP_SHL = 290,
    OP_SHR = 291,
    OP_GT = 292,
    OP_LT = 293,
    OP_GE = 294,
    OP_LE = 295,
    OP_EQ = 296,
    OP_NEQ = 297,
    OP_BIT_AND = 298,
    OP_BIT_XOR = 299,
    OP_BIT_OR = 300,
    OP_ASSIGN = 301,
    LOWER_THAN_ELSE = 302
  };
#endif
/* Tokens.  */
#define TYPE_INT 258
#define TYPE_CHAR 259
#define TYPE_FLOAT 260
#define INT 261
#define FLOAT 262
#define CHAR 263
#define ID 264
#define STRING 265
#define VOID 266
#define RETURN 267
#define IF 268
#define ELSE 269
#define WHILE 270
#define FOR 271
#define BREAK 272
#define CONTINUE 273
#define POINT 274
#define SEMI 275
#define COMMA 276
#define S_QUOTA 277
#define QUOTA 278
#define LP 279
#define RP 280
#define LB 281
#define RB 282
#define LC 283
#define RC 284
#define OP_STAR 285
#define OP_DIV 286
#define OP_MOD 287
#define OP_ADD 288
#define OP_SUB 289
#define OP_SHL 290
#define OP_SHR 291
#define OP_GT 292
#define OP_LT 293
#define OP_GE 294
#define OP_LE 295
#define OP_EQ 296
#define OP_NEQ 297
#define OP_BIT_AND 298
#define OP_BIT_XOR 299
#define OP_BIT_OR 300
#define OP_ASSIGN 301
#define LOWER_THAN_ELSE 302

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 9 "syntax.y" /* yacc.c:1909  */

    int type_int;
    float type_float;
    char type_char;
    char* type_str;

#line 155 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
