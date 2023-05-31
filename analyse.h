/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:analyse.h                         */
/* 模块功能说明:语法分析及保存语法分析树，语义分析及保存语义分析结果*/
/* 当前版本号:2.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.5.18                           */
/*************************************************/


#ifndef _ANALYZE_H_
#define _ANALYZE_H_


#include <string>
#include"globals.h"

using namespace std;


/*函数功能说明:语法分析将结果保存在对应文件路径*/
/*输入接口信息:argv:待分析代码文件路径 str:语法树保存文件路径  */
/*输出接口信息:                     */
/*调用注意事项:*/
extern int analyse(string argv, string str);

/*函数功能说明:语法分析将结果保存在对应文件路径*/
/*输入接口信息:argv:待分析代码文件路径 lists:语义分析保存文件路径  */
/*输出接口信息:                     */
/*调用注意事项:*/
extern int analyse_yuyi(string argv, string sem);

/*函数功能说明:中间分析将结果保存在对应文件路径*/
/*输入接口信息:argv:待分析代码文件路径  codefile:code保存文件路径 */
/*输出接口信息:                     */
/*调用注意事项:*/
extern int analyse_code(string argv,string codefile);


/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode *);

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode *);


#endif
