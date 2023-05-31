/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:scan.h                        */
/* 模块功能说明:词法分析获取token                   */
/* 当前版本号:1.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.19                            */
/*************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_
#include"globals.h"
#include <string>
using namespace std;
/* MAXTOKENLEN is the maximum size of a token */
#define MAXTOKENLEN 40

/* tokenString array stores the lexeme of each token */
extern char tokenString[MAXTOKENLEN + 1];

/*函数功能说明:返回源文件中的下一个标记    */
/*输入接口信息:void                                    */
/*输出接口信息:TokenType类型的值            */
/*调用注意事项:                  */
TokenType getToken(void);


extern  int linepos; /* 当前在LineBuf中的位置 */
extern  int bufsize; /* 缓冲区字符串的当前大小 */
extern  int EOF_flag;/* 是否修正EOF上的ungetNextChar行为的标记，1不修正，0修正 */

#endif
