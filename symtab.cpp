/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:symtab.cpp                          */
/* 模块功能说明:用于语义分析                  */
/* 当前版本号:1.0.1                              */
/* 作者:王可心                                    */
/* 完成日期: 2022.4.24                           */
/*************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"



/*SIZE是哈希表的大小*/
#define SIZE 211

/*移位是用作乘法器的二次幂在散列函数中*/
#define SHIFT 4

/*散列函数*/
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/*源的行号列表,引用变量的代码*/
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } *LineList;

/*存储在存储清单中的记录每个变量，包括名称，分配的内存位置，以及其中的行号列表,它出现在源代码中*/
typedef struct BucketListRec
   { char * name;
      char * type1;
     LineList lines;

     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
   } * BucketList;


/*哈希表*/
static BucketList hashTable[SIZE];


/*函数功能说明:程序st_insert插入行号和符号表中的内存位置，loc=仅在第一次，否则就被忽略了*/
/*输入接口信息:名称，行号列表，分配的内存位置 */
/*输出接口信息:void                */
/*调用注意事项:                    */
void st_insert( char * name, int lineno, int loc  ,ExpType type)
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* 没在列表中 */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    switch (type)
    {
    case Integer:
        l->type1="[int]";break;
    case FloaT:
        l->type1="[float]";break;
        break;
    case Void:
        l->type1="[void]"; break;
    case IntArray:
        l->type1="[int*]"; break;
    case FloatArray:
        l->type1="[float*]"; break;
    case ArrayUnit:
        l->type1="[*]"; break;
    }
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l; }
  else /* 在列表中找到了，只添加行号 */
  { 
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */


/*函数功能说明:函数st_lookup返回内存变量的位置，如果找不到，则为-1*/
/*输入接口信息:内存变量的名称 */
/*输出接口信息:int                */
/*调用注意事项:                    */
int st_lookup ( char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;
}
/*函数功能说明:函数st_lookup返回type1，如果找不到，则为0*/
/*输入接口信息:内存变量的名称 */
/*输出接口信息:int                */
/*调用注意事项:                    */
char * st_lookup1 ( char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return 0;
  else return l->type1;
}


/*函数功能说明:程序printSymTab打印格式化的符号表内容列表到列表文件*/
/*输入接口信息:要输出的列表文件 */
/*输出接口信息:void                */
/*调用注意事项:                    */
void printSymTab(FILE*list)
{ int i;
  fprintf(list,"Variable Name  Location   type         Line Numbers\n");
  fprintf(list,"-------------  --------  -------       ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(list,"%-14s ",l->name);
        fprintf(list,"%-8d  ",l->memloc);

         fprintf(list,"%-9s  ",l->type1);
         while (t != NULL)
         { fprintf(list,"%4d ",t->lineno);
           t = t->next;
         }
        fprintf(list,"\n");
        l = l->next;
      }
    }
  }
} /* printSymTab */
