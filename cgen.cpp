/****************************************************/
/* 版权信息:2022 20183231114王可心                   */
/* 文件名:cgen.h                                     */
/* 模块功能说明:用于MiniC编译器的中间代码生成器接口                  */
/* 当前版本号:1.0.1                                */
/* 作者:王可心                            */
/* 完成日期:2022.5.23                                    */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "code.h"
#include "cgen.h"
#include<iostream>

/*tmpOffset是temp的内存偏移量每次温度降低时，它都会减小存储，并在再次加载时递增*/
static int tmpOffset = 0;

/*内部递归代码生成器原型*/
static void cGen (TreeNode * tree);

/*函数功能说明：genStmt在语句节点生成代码*/
/*输入接口信息:树节点  */
/*输出接口信息:                     */
/*调用注意事项:*/
static void genStmt(TreeNode * tree)
{
    TreeNode * p1, *p2, *p3;
    int savedLoc1, savedLoc2, currentLoc;
    int loc;
    switch (tree->kind.stmt)
    {

    case IfK://类似tiny中的if语句

        if (TraceCode) emitComment("-> if") ;
        p1 = tree->child[0] ;
        p2 = tree->child[1] ;

        /* generate code for test expression */
        cGen(p1);
        savedLoc1 = emitSkip(1) ;
        emitComment("if: jump to else belongs here");

        /* recurse on part */
        cGen(p2);
        savedLoc2 = emitSkip(1);
        emitComment("if: jump to end belongs here");
        currentLoc = emitSkip(0) ;
        emitBackup(savedLoc1) ;
        emitRM_Abs("JEQ",ac,currentLoc,"if: jmp to else");
        emitRestore() ;
        if(tree->child[2]!=NULL)
        {
        p3 = tree->child[2]->child[0] ;
        /* recurse on else part */
        cGen(p3);
        currentLoc = emitSkip(0) ;
        emitBackup(savedLoc2) ;
        emitRM_Abs("LDA",pc,currentLoc,"jmp to end") ;
        emitRestore() ;}
        if (TraceCode)  emitComment("<- if") ;
        break; /* if_k */

    case DoK://类似于tiny中的repeat语句但until和while的循环条件不同
        if (TraceCode) emitComment("-> dowhile") ;
        p1 = tree->child[0] ;
        p2 = tree->child[1]->child[0] ;
        savedLoc1 = emitSkip(0);
        emitComment("dowhile: jump after body comes back here");
        /* generate code for body */
        cGen(p1);
        /* generate code for test */
        cGen(p2);
        emitRM_Abs("JNE",ac,savedLoc1,"dowhile: jmp back to body");
        if (TraceCode)  emitComment("<- dowhile") ;
        break; /* dowhile */
    case VarDefinitionK://单个变量声明 需要查找地址
           if (TraceCode) emitComment("-> var-definition");
           loc = st_lookup(tree->attr.name);//查找变量的地址
           emitRM("ST", ac, loc, gp, "var-definition store value");//中间代码
           if (TraceCode)  emitComment("<- var-definition");
           break;

       case FunctionK://单个函数声明 需要查找地址
           if (TraceCode) emitComment("-> function-definition");
           loc = st_lookup(tree->attr.name);//查找函数的地址
           emitRM("ST", ac, loc, gp, "function-definition store function"); //中间代码
           p1 = tree->child[0];
           p2 = tree->child[1];
           cGen(p1);
           cGen(p2);
           if (TraceCode)  emitComment("<- function-definition");
           break;

       case ParamListK://函数内多个参数 不需要查找地址
           if (TraceCode) emitComment("-> paramter-list");
           p1 = tree->child[0];
           cGen(p1);

           for (int i = 1; i < MAXCHILDREN; i++)//因为不可以为empty所以以1开始
           {
               p1 = tree->child[i];
               if (p1 != NULL)
               {cGen(p1);

               }

           }
           if (TraceCode)  emitComment("<- paramter-list");
           break;

       case ParamK://函数声明参数列表单个参数 需要查找地址
           if (TraceCode) emitComment("-> paramter");
           if (tree->type == Void)
               break;
           else
           {
               loc = st_lookup(tree->attr.name);
               emitRM("ST", ac, loc, gp, "paramter: store the value of a parameter list");
           }
           if (TraceCode)  emitComment("<- paramter");
           break;

       case ComK://函数体 不需要查找地址
           if (TraceCode) emitComment("-> compound-stmt");
           p1 = tree->child[0];
           p2 = tree->child[1];
           cGen(p1);
           cGen(p2);
           if (TraceCode)  emitComment("<- compound-stmt");
           break;

       case LocalDefinitionK://多个变量的声明 不需要查找地址
           if (TraceCode) emitComment("-> local-definition");
           for (int i = 0; i < MAXCHILDREN; i++)//因为可以为empty所以以0开始
           {
               p1 = tree->child[i];
               if (p1 != NULL)
                   cGen(p1);
           }
           if (TraceCode)  emitComment("<- local-definition");
           break;

       case StmtListK://函数体内的语句 不需要查找地址
           if (TraceCode) emitComment("-> statement-list");
           for (int i = 0; i < MAXCHILDREN; i++)//因为可以为empty所以以0开始
           {
               p1 = tree->child[i];
               if (p1 != NULL)
                   cGen(p1);
           }
           if (TraceCode)  emitComment("<- statement-list");
           break;
    case ReturnK://return返回语句 需要查找地址
        if (TraceCode) emitComment("-> return");
        if(tree->child[0]!=NULL)
        {p1 = tree->child[0];
        cGen(p1);}
        //loc = st_lookup(p1->attr.name);//查找返回变量的地址
        //emitRM("ST", ac, loc, gp, "return: store value"); //生成中间代码
        if (TraceCode)  emitComment("<- return");
        break;
    case CallK://return返回语句 需要查找地址
        if(tree->attr.name=="output")
        {
            cGen(tree->child[0]);
            emitRO("OUT", ac, 0, 0, "write ac"); //生成中间代码
        }
        else if(tree->attr.name=="input")
        {
            emitRO("IN", ac, 0, 0, "read integer value"); //生成中间代码
            loc = st_lookup(tree->attr.name);
            emitRM("ST",ac,loc,gp,"input: store value");
        }
        else
               {
                   p1 = tree->child[0];
                   if(p1!=0)
                        {cGen(p1);
                   loc = st_lookup(tree->attr.name);//查找调用函数名所在地址
                   emitRM("ST", ac, loc, gp, "call: store value");}//生成中间代码

               }

        break;
    case ArgK://多个调用 不需要查找地址
        if (TraceCode) emitComment("-> arguments");
        for (int i = 0; i < MAXCHILDREN; i++)//因为可以为empty所以以0开始
        {
            p1 = tree->child[i];
            if (p1 != NULL)
                cGen(p1);
        }
        if (TraceCode)  emitComment("<- arguments");
        break;
    case ArgListK://多个调用 不需要查找地址
        if (TraceCode) emitComment("-> argument_list");
        p1 = tree->child[0];
            cGen(p1);
        for (int i = 1; i < MAXCHILDREN; i++)
        {
            p1 = tree->child[i];
            if (p1 != NULL)
                cGen(p1);
        }
        if (TraceCode)  emitComment("<- argument_list");
        break;
    case AssignK://类似tiny赋值语句 需要查找地址
        if (TraceCode) emitComment("-> assign") ;
        /* generate code for rhs */
        cGen(tree->child[0]);
        /* now store value */
        loc = st_lookup(tree->attr.name);
        emitRM("ST",ac,loc,gp,"assign: store value");
        if (TraceCode)  emitComment("<- assign") ;
        break; /* assign_k */

    default:
        break;
    }
} /* genStmt */


/*过程genExp在表达式节点生成代码*/
/*输入接口信息:树节点  */
/*输出接口信息:                     */
/*调用注意事项:*/
static void genExp( TreeNode * tree)
{ int loc;
  TreeNode * p1, * p2;
  switch (tree->kind.exp) {

    case ConstK :
      if (TraceCode) emitComment("-> Const") ;
      /* gen code to load  constant using LDC */
      emitRM("LDC",ac,tree->attr.val,0,"load const");
      if (TraceCode)  emitComment("<- Const") ;
      break; /* ConstK */

    case IdK :
      if (TraceCode) emitComment("-> Id") ;
      if (tree->type == ArrayUnit||tree->type==IntArray||tree->type==FloatArray)
          {
              cGen(tree->child[0]);//数组的索引
        loc = st_lookup(tree->attr.name);
              emitRM("LD", ac, loc, gp, "load array unit value");
          }
          else
          {
              loc = st_lookup(tree->attr.name);
              emitRM("LD", ac, loc, gp, "load id value");
          }
      if (TraceCode)  emitComment("<- Id");
      break; /* IdK */

    case OpK :
         if (TraceCode) emitComment("-> Op") ;
         p1 = tree->child[0];
         p2 = tree->child[1];
         /* gen code for ac = left arg */
         cGen(p1);
         /* gen code to push left operand */
         emitRM("ST",ac,tmpOffset--,mp,"op: push left");
         /* gen code for ac = right operand */
         cGen(p2);
         /* now load left operand */
         emitRM("LD",ac1,++tmpOffset,mp,"op: load left");
         switch (tree->attr.op) {
            case PLUS :
               emitRO("ADD",ac,ac1,ac,"op +");
               break;
            case MINUS :
               emitRO("SUB",ac,ac1,ac,"op -");
               break;
            case TIMES :
               emitRO("MUL",ac,ac1,ac,"op *");
               break;
            case OVER :
               emitRO("DIV",ac,ac1,ac,"op /");
               break;
            case MOOD :
               emitRO("MOD",ac,ac1,ac,"op %");
               break;
            case LT :
               emitRO("SUB",ac,ac1,ac,"op <") ;
               emitRM("JLT",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case RT :
               emitRO("SUB",ac,ac1,ac,"op >") ;
               emitRM("JRT",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case GE :
               emitRO("SUB",ac,ac1,ac,"op >=") ;
               emitRM("JGE",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case LE :
               emitRO("SUB",ac,ac1,ac,"op <=") ;
               emitRM("JLE",ac,2,pc,"br if true") ;
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case EQ :
               emitRO("SUB",ac,ac1,ac,"op ==") ;
               emitRM("JEQ",ac,2,pc,"br if true");
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            case NEQ :
               emitRO("SUB",ac,ac1,ac,"op !=") ;
               emitRM("JNE",ac,2,pc,"br if true");
               emitRM("LDC",ac,0,ac,"false case") ;
               emitRM("LDA",pc,1,pc,"unconditional jmp") ;
               emitRM("LDC",ac,1,ac,"true case") ;
               break;
            default:
               emitComment("BUG: Unknown operator");
               break;
         } /* case op */
         if (TraceCode)  emitComment("<- Op") ;
         break; /* OpK */

    default:
      break;
  }
} /* genExp */

/*过程cGen通过树遍历方式递归生成代码*/
/*输入接口信息:树节点 */
/*输出接口信息:                     */
/*调用注意事项:*/
static void cGen( TreeNode * tree)
{ if (tree != NULL)

  {
        switch (tree->nodekind) {
      case StmtK:
        genStmt(tree);
        break;
      case ExpK:
        genExp(tree);
        break;
      default:
        break;
    }
    cGen(tree->sibling);
  }
}

/**********************************************/
/*代码生成器的主要功能*/
/**********************************************/
/*函数功能说明：codeGen通过遍历语法树创建文件*/
/*输入接口信息:树根和存储的文件名 */
/*输出接口信息:                     */
/*调用注意事项:*/
void codeGen(TreeNode * syntaxTree, char * codefile)
{  char * s = (char *)malloc(strlen(codefile) + 7);
   strcpy(s,"File: ");
   strcat(s,codefile);
   emitComment(" Compilation to TM Code");
   emitComment(s);
   /* generate standard prelude */
   emitComment("Standard prelude:");
   emitRM("LD",mp,0,ac,"load maxaddress from location 0");
   emitRM("ST",ac,0,ac,"clear location 0");
   emitComment("End of standard prelude.");
   /* generate code for TINY program */
   cGen(syntaxTree);
   /* finish */
   emitComment("End of execution.");
   emitRO("HALT",0,0,0,"");
}
