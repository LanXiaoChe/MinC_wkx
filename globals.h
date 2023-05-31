/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:globals.h                            */
/* 模块功能说明:定义关键字、保留字等全局变量            */
/* 当前版本号:1.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.19                            */
/*************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <map>
#include <QString>


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

typedef enum
/* token类型 */
{
     /* book-keeping tokens */
    ENDFILE, ERROR_INP,
    /* reserved words */
    IF, ELSE, Int, Float,RETURN, VoiD,Do, WHILE,
   /* multicharacter tokens */
    ID, NUM,
    /* special symbols */
    /* UNLINE:'_',PLUS:'+',MINUS:'-',TIMES:'*',OVER:'/', MOOD:'%',LT:'<',RT:'>',GE:'>=',LE:'<=',EQ:'==', NEQ:'!=', */
    /* ASSIGN:'=',SEMI:';',DOT:',',LPAREN:'(',RPAREN:')'，LBRACKET:'[',RBRACKET:']',LBRACE:'{',   RBRACE:'}'*/
    /*RNOTE:'右注释' ,LNOTE:'左注释' */
    UNLINE,MOOD,ASSIGN, GE, LE, EQ, NEQ, LBRACKET, RBRACKET, LBRACE, RBRACE, LT, RT, PLUS, MINUS,
    TIMES, OVER, LPAREN, RPAREN, SEMI, DOT, LNOTE, RNOTE
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* result; /*词法分析保存文件*/
extern FILE* code; /* code text file for TM simulator */
extern FILE* list;/*语义分析保存文件*/
extern int lineno; /* source line number for listing */
extern char sourceFile[120];

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/
typedef enum { StmtK, ExpK } NodeKind;
typedef enum { IfK, LocalDefinitionK, ReturnK, ExpStmtK, AssignK,
               ElseK,DoK,WhileK, FunctionK, VarDefinitionK, DefinitionK,
               StmtListK, ParamListK, CallK, ArgListK, ParamK, ComK, ArgK, } StmtKind;
typedef enum { OpK, ConstK, IdK } ExpKind;

/* ExpType is used for type checking */
/* IntArray:Int数组类型,FloatArray:Float数组类型, ArrayUnit:数组单元,Boolean:布尔类型*/
typedef enum { Void,Integer, FloaT,IntArray, FloatArray,ArrayUnit ,Boolean} ExpType;
#define MAXCHILDREN 10


typedef struct treeNode
{
    struct treeNode * child[MAXCHILDREN];
    struct treeNode * sibling;
    int lineno;
    NodeKind nodekind;
    union { StmtKind stmt; ExpKind exp; } kind;
    union {
        TokenType op;
        float val;
        char * name;
    } attr;
    ExpType type; /* for type checking of exps */
} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif

