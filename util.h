/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:util.h                         */
/* 模块功能说明:打印语法树以及词法分析和语法分析涉及到的函数*/
/* 当前版本号:1.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.19                            */
/*************************************************/


#ifndef _UTIL_H_
#define _UTIL_H_
#include "globals.h"

/*函数功能说明:将标记及其类型打印到语法树文件*/
/*输入接口信息:TokenType:token，const char*:tokenString*/
/*输出接口信息:void*/
/*调用注意事项:*/
void printToken(TokenType, const char*);

/*函数功能说明:为语法树创建一个语句节点  */
/*输入接口信息:StmtKind类型:语句节点的语句类型  */
/*输出接口信息:TreeNode类型的节点  */
/*调用注意事项:  */
TreeNode * newStmtNode(StmtKind);

/*函数功能说明:创建用于语法树构造的表达式节点  */
/*输入接口信息:ExpKind类型:表达式节点的类型  */
/*输出接口信息:TreeNode类型的表达式节点地址  */
/*调用注意事项:  */
TreeNode * newExpNode(ExpKind);

/*函数功能说明:分配并生成现有字符串的新副本   */
/*输入接口信息:char *类型的数据  */
/*输出接口信息:char *类型的数据  */
/*调用注意事项:  */
char * copyString(char *);

/*函数功能说明:打印语法树   */
/*输入接口信息:TreeNode *:表示需要打印的语法树节点  */
/*输出接口信息:void  */
/*调用注意事项:  */
void printTree(TreeNode *);

#endif

