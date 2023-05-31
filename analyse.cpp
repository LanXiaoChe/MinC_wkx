/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:analyse.cpp                          */
/* 模块功能说明:语法分析及保存语法分析树进行语义分析                   */
/* 当前版本号:3.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.5.24                           */
/*************************************************/

#include "globals.h"
#include"analyse.h"
#include "util.h"
#include "parse.h"
#include "scan.h"
#include"symtab.h"
#include"cgen.h"
#include <string>
#include<stdio.h>
using namespace std;



int lineno = 0;
FILE * source;
FILE * listing;//语法树保存文件
FILE * result;//词法分析结果保存文件
FILE* list;//语义分析结果保存文件
FILE* code; /* code text file for TM simulator */


int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

/*函数功能说明:语法分析将结果保存在对应文件路径*/
/*输入接口信息:argv:待分析代码文件路径 str:语法树保存文件路径  */
/*输出接口信息:                     */
/*调用注意事项:*/
TreeNode * syntaxTree;
int analyse(string argv, string str)
{
    lineno = 0;

    char pgm[120]; /* source code file name */
    strcpy(pgm, argv.c_str());
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".tny");
    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    listing = fopen(str.c_str(), "w");
    fprintf(listing, "\nMiniC TINY Syntax Analyse!\n");
    //int linepos = 0;
    //int bufsize = 0;
    EOF_flag = FALSE;
    syntaxTree = parse();
    fprintf(listing, "\nMiniC TINY Syntax tree:\n");
    fprintf(listing, "program\n");
    fprintf(listing, "definition_list\n");
    fprintf(listing, "definition\n");
    printTree(syntaxTree);
    fclose(listing);
    fclose(source);
    return 0;
}
char *rt1[1000][1000];
int r1;
int analyse_yuyi(string argv, string sem)
{
    lineno = 0;
   // TreeNode * syntaxTree;
    char pgm[120]; /* source code file name */
    strcpy(pgm, argv.c_str());
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".tny");
    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    list = fopen(sem.c_str(), "w");
    fprintf(list, "\nMiniC TINY yuyi Analyse!\n");
    //int linepos = 0;
    //int bufsize = 0;
    EOF_flag = FALSE;
    //syntaxTree = parse();
    for (int a=0;a<1000;a++)
    {
        for (int b=0;b<1000;b++)
        {
             rt1[a][b]=rtchange(a,b);
        }
    }
    r1=rtchange1();
    buildSymtab(syntaxTree);
    fprintf(list, "\check!\n");
    typeCheck(syntaxTree);
        if(Error==FALSE)
            fprintf(list, "\nright!\n");
    fclose(list);
    fclose(source);
    return 0;
}


/*函数功能说明:中间分析将结果保存在对应文件路径*/
/*输入接口信息:argv:待分析代码文件路径 */
/*输出接口信息:                     */
/*调用注意事项:*/
extern int analyse_code(string argv,string codefile)
{
    lineno = 0;
    //TreeNode * syntaxTree;
    char pgm[120]; /* source code file name */
    strcpy(pgm, argv.c_str());
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".tny");
    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    code = fopen(codefile.c_str(), "w");
    QByteArray ode = codefile.c_str();
    char* f =ode.data();
    fprintf(code, "\nMiniC TINY code Analyse!\n");
    //int linepos = 0;
    //int bufsize = 0;
    EOF_flag = FALSE;
    codeGen(syntaxTree, f);
    fclose(code);
    fprintf(code, "\check!\n");
    fclose(code);
    fclose(source);
    return 0;
}



//MininC中间代码生成
static char * funcName;
static int preserveLastScope = FALSE;

/* counter for variable memory locations */
static int location = 0;
static int locationn = 0;
/*函数功能说明:过程遍历是一种通用的递归遍历，语法树遍历例程：它将预处理应用于预处理和后处理，按顺序排列到t所指的树上*/
/*输入接口信息:树根及左右结点  */
/*输出接口信息:                     */
static void traverse( TreeNode * t,void (* preProc) (TreeNode *),void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}


/*函数功能说明:nullProc是一个不做任何事情的过程*/
/*输入接口信息:树结点  */
/*输出接口信息:                     */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

int q;
/*函数功能说明:过程节点插入，存储在t中的符号表中*/
/*输入接口信息:树结点  */
/*输出接口信息:                     */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK://函数
      switch (t->kind.stmt)
      { case VarDefinitionK:
        case FunctionK:
        case ParamK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++,t->type);
          else
          /* already in table, so ignore location,
             add line number of use only */
            st_insert(t->attr.name,t->lineno,0,t->type);
          break;
      case CallK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++,t->type);
          else
          /* already in table, so ignore location,
             add line number of use only */
            st_insert(t->attr.name,t->lineno,0,t->type);
          break;

        default:
            break;
      }
        break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          if (st_lookup(t->attr.name) == -1)
          /* not yet in table, so treat as new definition */
            st_insert(t->attr.name,t->lineno,location++,t->type);
          else
          /* already in table, so ignore location,
             add line number of use only */
            st_insert(t->attr.name,t->lineno,0,t->type);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}



/*函数功能说明:函数buildSymtab按语法树的前序遍历构造符号表*/
/*输入接口信息:树结点  */
/*输出接口信息:                     */
void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if (TraceAnalyze)
  { fprintf(list,"\nSymbol table:\n\n");
    printSymTab(list);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(list,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}
char *ds,*yu;

/*函数功能说明:checkNode:在单个树节点上进行类型检查*/
/*输入接口信息:树结点  */
/*输出接口信息:                     */
/* UNLINE:'_',PLUS:'+',MINUS:'-',TIMES:'*',OVER:'/', MOOD:'%',LT:'<',RT:'>',GE:'>=',LE:'<=',EQ:'==', NEQ:'!=', */
/* ASSIGN:'=',SEMI:';',DOT:',',LPAREN:'(',RPAREN:')'，LBRACKET:'[',RBRACKET:']',LBRACE:'{',   RBRACE:'}'*/
/*RNOTE:'右注释' ,LNOTE:'左注释' */
static void checkNode(TreeNode * t)
{
    switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if (((t->child[0]->type != Integer)&&(t->child[0]->type != FloaT) &&(t->child[0]->type != IntArray)&&(t->child[0]->type != FloatArray)&&(t->child[0]->type != ArrayUnit))||(
              (t->child[1]->type != Integer)&&(t->child[1]->type != FloaT)&&(t->child[1]->type != FloatArray)&&(t->child[1]->type != IntArray)&&(t->child[1]->type != ArrayUnit)))
              typeError(t,"OpK applied to non-integer or non-Float or non-Array");
          switch(t->attr.op)
          {
            case PLUS:
            case MINUS:
            case TIMES:
            case OVER:
                if(t->child[0]->type==FloaT||t->child[0]->type==FloatArray||t->child[1]->type==FloaT||t->child[1]->type==FloatArray)
                    t->type=FloaT;
                else
                    t->type=Integer;
                break;
            case ASSIGN:
              ds=st_lookup1 ( t->attr.name );
              if(strcmp(ds,"[int]")==0)
                  t->type=Integer;
              else if(strcmp(ds,"[float]")==0)
                   t->type=FloaT;
              else if(strcmp(ds,"[int*]")==0)
                  t->type=Integer;
              else if(strcmp(ds,"[float*]")==0)
                  t->type=FloaT;
              break;
            case MOOD:
              if(t->child[0]->kind.exp!=ConstK&&t->child[0]->kind.exp!=OpK)
              {
              ds=st_lookup1 ( t->child[0]->attr.name );
              if(strcmp(ds,"[int]")==0)
                  t->child[0]->type=Integer;
              else if(strcmp(ds,"[float]")==0)
                   t->child[0]->type=FloaT;
              else if(strcmp(ds,"[int*]")==0)
                  t->child[0]->type=IntArray;
              else if(strcmp(ds,"[float*]")==0)
                  t->child[0]->type=FloatArray;
              }
              if((t->child[0]->type==Integer||t->child[0]->type==IntArray)&&(t->child[0]->type==Integer||t->child[1]->type==IntArray))
                t->type=Integer;
              else
                  typeError(t,"MOOD is not int");
                break;
            case LT:
            case RT:
            case GE:
            case LE:
            case EQ:
            case NEQ:
                t->type=Boolean;
                break;
            default:
                break;
           }
          break;
        case ConstK:
            if(t->type==FloaT||t->type==Integer)
                break;
            else
                typeError(t,"Const is not right");
            break;
        case IdK:
          if(t->type==ArrayUnit)
          {
              ds=st_lookup1 ( t->attr.name );
              if(strcmp(ds,"[int*]")==0)
                  t->type=IntArray;
              else if(strcmp(ds,"[float*]")==0)
                  t->type=FloatArray;
              else
                  typeError(t,"Id is not right");
          }
          else
          {ds=st_lookup1 ( t->attr.name );
          if(strcmp(ds,"[int]")==0)
              t->type=Integer;
          else if(strcmp(ds,"[float]")==0)
              t->type=FloaT;
          else if(strcmp(ds,"[void]")==0)
              typeError(t,"Id is not right");
          }
          if(t->child[0]!=NULL)
          {
              if(t->child[0]->kind.exp!=ConstK&&t->child[0]->kind.exp!=OpK)
              {
              ds=st_lookup1 ( t->child[0]->attr.name );
              if(strcmp(ds,"[int]")==0)
                  t->child[0]->type=Integer;
              else if(strcmp(ds,"[float]")==0)
                   t->child[0]->type=FloaT;
              else if(strcmp(ds,"[int*]")==0)
                  t->child[0]->type=IntArray;
              else if(strcmp(ds,"[float*]")==0)
                  t->child[0]->type=FloatArray;
              }
              if(t->child[0]->type!=Integer)
              {
                  typeError(t,"Id[] is not right");
              }
          }
            break;
        default:
          break;
        }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type!=Boolean)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case DoK:
          if (t->child[1]->child[0]->type!=Boolean)
               typeError(t->child[0],"dowhile test is not Boolean");
          break;
        case ReturnK:
          if(t->type==Void)
          {
              if(t->child[0]!=NULL)
              {
                  typeError(t->child[0],"return test is not right");
                  break;
              }
          }
          if(t->child[0]->kind.exp!=ConstK&&t->child[0]->kind.exp!=OpK)
          {
              ds=st_lookup1 ( t->child[0]->attr.name );

          }
          else
          {
              if(t->child[0]->type==Integer)
                ds="[int]";
              else if(t->child[0]->type==FloaT)
                ds="[float]";
          }
          if(t->type==Integer)
            yu="[int]";
          else if(t->type==FloaT)
            yu="[float]";
         if (strcmp(ds,yu))
              typeError(t->child[0],"return test is not right");
          break;
      case AssignK:
           ds=st_lookup1 ( t->attr.name );

           if(strcmp(ds,"[int]")==0)
               t->type=Integer;
           else if(strcmp(ds,"[float]")==0)
               t->type=FloaT;
           else if(strcmp(ds,"[int*]")==0)
               t->type=Integer;
           else if(strcmp(ds,"[float*]")==0)
               t->type=FloaT;
           if(t->child[0]->kind.exp!=ConstK&&t->child[0]->kind.exp!=OpK)
           {
           ds=st_lookup1 ( t->child[0]->attr.name );
           if(strcmp(ds,"[int]")==0)
               t->child[0]->type=Integer;
           else if(strcmp(ds,"[float]")==0)
                t->child[0]->type=FloaT;
           else if(strcmp(ds,"[float*]")==0)
                t->child[0]->type=FloaT;
           else if(strcmp(ds,"[int*]")==0)
                t->child[0]->type=Integer;
           }
           if(t->type==t->child[0]->type)
               break;
           else
               typeError(t, "assign list is not right");

        break;
        case FunctionK:

          if(t->type != Integer&&t->type!=FloaT&&t->type!=Void)
              typeError(t, "function argument list is not right");
          if (t->child[0] == NULL)//函数参数列表为空
              {
              break;}
          else if (t->child[0]->child[0]->type == Void)//函数参数列表为Void
          {

              break;
          }
          else//多参数
          {
              int i = 0;
              while (t->child[0]->child[i] != NULL)
              {

                  if (t->child[0]->child[i]->type!=FloaT&&t->child[0]->child[i]->type!=Integer&&t->child[0]->child[i]->type!=FloatArray&&t->child[0]->child[i]->type!=IntArray)
                        typeError(t->child[0]->child[i], "the parsing of a function argument list is not right");


                  i++;
              }

          }

          break;
        case VarDefinitionK://变量
          if (t->type!=FloaT&&t->type!=Integer&&t->type!=IntArray&&t->type!=FloatArray)
              typeError(t, "the  VarDefinitionK list is not right");
          if(t->type==IntArray||t->type==FloatArray)
          {
              if(t->child[0]->type!=Integer)
                   typeError(t, "the  VarDefinitionK [] list is not right");
          }
          break;
        case CallK://
              if (strcmp(t->attr.name,"output")==0)//对ouput检验
              {
                  int j = 0;
                  if(t->child[0]!=NULL)
                  {
                  while (t->child[0]->child[0]->child[j]!= NULL)
                  {
                      j++;
                  }
                  if(j==1)
                  {
                      char *dd=st_lookup1 ( t->child[0]->child[0]->child[j-1]->attr.name );
                      if(strcmp(dd,"[int]")==0||strcmp(dd,"[int*]")==0)
                      {

                      }
                  }
                  else
                  {
                       typeError(t, "the  output type is not right");
                  }
                  }
              }
              else if (strcmp(t->attr.name,"input")==0)//对input检验
              {
                  int o = 0;
                  while (t->child[0]!= NULL)
                  {
                      o++;
                  }
                  if(o!=0)
                  {
                      typeError(t, "the  input type is not right");
                  }
              }
              else
              {


                  int l=0;
                  for(int i=0;i<r1;i++)
                  {
                      if(strcmp(t->attr.name,rt1[i][0])==0)
                      {
                          l=i;
                      }
                  }
                  if(l==0&&strcmp(t->attr.name,rt1[l][0]))
                  {
                      typeError(t, "the CallK para  is not right");
                  }
                  if(t->child[0]==NULL)
                  {
                      if(strcmp( "@",rt1[l][1])!=0 )
                       { typeError(t, "the CallK para  is not right");

                      }
                      break;

                  }

                  int s=0;

                  while((t->child[0]->child[0]->child[s]!=NULL)&&(strcmp( "@",rt1[l][s+1])))
                  {
                      char *dd;
                      if(t->child[0]->child[0]->child[s]->kind.exp!=ConstK&&t->child[0]->child[0]->child[s]->kind.exp!=OpK)
                      {
                        dd=st_lookup1 ( t->child[0]->child[0]->child[s]->attr.name );
                      }
                      else
                      {
                        if(t->child[0]->child[0]->child[s]->type==Integer)
                            dd="[int]";
                        else if(t->child[0]->child[0]->child[s]->type==FloaT)
                            dd="[float]";
                      }
                      if(strcmp(dd,rt1[l][s+1])){typeError(t, "the CallK para is not right");
                      }

                      s++;
                  }
                  int e=0;
                  while(t->child[0]->child[0]->child[e]!=NULL)
                  {
                      e++;
                  }
                  if(strcmp("@",rt1[l][e])!=0&&strcmp("@",rt1[l][e+1])==0)
                  {}
                  else {
                      typeError(t, "the CallK para  num is not right");
                  }

              }
          break;

              }
    default:
      break;

    }
}


/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{traverse(syntaxTree,nullProc,checkNode);
}
