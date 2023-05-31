/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:symtab.h                          */
/* 模块功能说明:用于语义分析                  */
/* 当前版本号:1.0.1                              */
/* 作者:王可心                                    */
/* 完成日期: 2022.4.24                           */
/*************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
/*函数功能说明:程序st_insert插入行号和符号表中的内存位置，loc=仅在第一次，否则就被忽略了*/
/*输入接口信息:名称，行号列表，分配的内存位置 */
/*输出接口信息:void                */
/*调用注意事项:                    */
void st_insert( char * name, int lineno, int loc ,ExpType type);



/*函数功能说明:函数st_lookup返回内存变量的位置，如果找不到，则为-1*/
/*输入接口信息:内存变量的名称 */
/*输出接口信息:int                */
/*调用注意事项:                    */
int st_lookup ( char * name );

char * st_lookup1 ( char * name );

/*函数功能说明:程序printSymTab打印格式化的符号表内容列表到列表文件*/
/*输入接口信息:要输出的列表文件 */
/*输出接口信息:void                */
/*调用注意事项:                    */
void printSymTab(FILE* list);

#endif
