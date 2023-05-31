/****************************************************/
/* 版权信息:2022 20183231114王可心                 */
/* 文件名:code.cpp                                     */
/* 模块功能说明:用于MiniC编译器的中间代码生成器接口                  */
/* 当前版本号:1.0.1                                */
/* 作者:王可心                                */
/* 完成日期:2022.5.18                                     */
/****************************************************/

#include "globals.h"
#include "code.h"

/*当前指令发射的TM位置号*/
static int emitLoc = 0 ;

/*迄今为止发射的最高TM位置
与Emit Skip一起使用，
emitBackup和emitRestore*/
static int highEmitLoc = 0;

/*函数功能说明:代码发射实用程序：过程emitComment代码文件中带有注释c时打印注释行*/
/*输入接口信息:指针c                              */
/*输出接口信息:void           */
/*调用注意事项:               */
void emitComment( char * c )
{ if (TraceCode) fprintf(code,"* %s\n",c);}

/*函数功能说明：程序emitRO只发出一个寄存器
*TM指令
*op=操作码
*r=目标寄存器
*s=第一源寄存器
*t=第二源寄存器
*c=如果TraceCode为真，则要打印的注释*/
/*输入接口信息:操作码，目标寄存器，第一源寄存器，第二源寄存器*/
/*输出接口信息:void           */
/*调用注意事项:               */
void emitRO( char *op, int r, int s, int t, char *c)
{ fprintf(code,"%3d:  %5s  %d,%d,%d ",emitLoc++,op,r,s,t);
  if (TraceCode) fprintf(code,"\t%s",c) ;
  fprintf(code,"\n") ;
  if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
} /* emitRO */

/*函数功能说明：过程emitRM向内存发送寄存器
*TM指令
*op=操作码
*r=目标寄存器
*d=偏移量
*s=基址寄存器
*c=如果TraceCode为真，则要打印的注释*/
/*输入接口信息:操作码，目标寄存器，偏移量，基址寄存器*/
/*输出接口信息:void           */
/*调用注意事项:               */
void emitRM( char * op, int r, int d, int s, char *c)
{ fprintf(code,"%3d:  %5s  %d,%d(%d) ",emitLoc++,op,r,d,s);
  if (TraceCode) fprintf(code,"\t%s",c) ;
  fprintf(code,"\n") ;
  if (highEmitLoc < emitLoc)  highEmitLoc = emitLoc ;
} /* emitRM */

/*函数功能说明：函数跳过“多少”代码后面的补丁位置。它也返回当前代码位置*/
/*输入接口信息:跳过的代码补丁位置*/
/*输出接口信息:void           */
/*调用注意事项:               */
int emitSkip( int howMany)
{  int i = emitLoc;
   emitLoc += howMany ;
   if (highEmitLoc < emitLoc)  highEmitLoc = emitLoc ;
   return i;
} /* emitSkip */


/*函数功能说明：备份过程，备份到loc=先前跳过的位置*/
/*输入接口信息:先前跳过的位置*/
/*输出接口信息:void           */
/*调用注意事项:               */
void emitBackup( int loc)
{ if (loc > highEmitLoc) emitComment("BUG in emitBackup");
  emitLoc = loc ;
} /* emitBackup */

/*函数功能说明：还原过程还原当前代码位置到之前未分配职位的最高位置*/
/*输入接口信息:void*/
/*输出接口信息:void           */
/*调用注意事项:               */
void emitRestore(void)
{ emitLoc = highEmitLoc;}

/*函数功能说明：程序emitRM_Abs转换绝对参考
*在发射信号时指向pc的相对参考
*寄存器到存储器TM指令
*op=操作码
*r=目标寄存器
*a=内存中的绝对位置
*c=如果TraceCode为真，则要打印的注释*/
/*输入接口信息:操作码，目标寄存器，内存中的绝对位置，要打印的注释*/
/*输出接口信息:void           */
/*调用注意事项:               */
void emitRM_Abs( char *op, int r, int a, char * c)
{ fprintf(code,"%3d:  %5s  %d,%d(%d) ",
               emitLoc,op,r,a-(emitLoc+1),pc);
  ++emitLoc ;
  if (TraceCode) fprintf(code,"\t%s",c) ;
  fprintf(code,"\n") ;
  if (highEmitLoc < emitLoc) highEmitLoc = emitLoc ;
} /* emitRM_Abs */
