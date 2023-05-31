/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:util.cpp                          */
/* 模块功能说明:打印语法树以及词法分析和语法分析涉及到的函数*/
/* 当前版本号:1.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.19                            */
/*************************************************/


#include "globals.h"
#include "util.h"

/*函数功能说明:printToken将标记及其类型打印到语法树文件*/
/*输入接口信息:TokenType:token，const char*:tokenString*/
/*输出接口信息:void*/
/*调用注意事项:*/
void printToken(TokenType token, const char* tokenString)
{
    switch (token)
    {
    case IF:
    case ELSE:
    case Int:
    case Float:
    case RETURN:
    case VoiD:
    case WHILE:
    case Do:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;
    case LT: fprintf(listing, "<\n"); break;
    case RT: fprintf(listing, ">\n"); break;
    case EQ: fprintf(listing, "==\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case SEMI: fprintf(listing, ";\n"); break;
    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case MOOD:fprintf(listing, "qiuyu\n"); break;
    case UNLINE:fprintf(listing, "_\n"); break;
    case ENDFILE: fprintf(listing, "EOF\n"); break;
    case NEQ: fprintf(listing, "!=\n"); break;
    case ASSIGN:fprintf(listing, "=\n"); break;
    case DOT:fprintf(listing, ",\n"); break;
    case LBRACKET:fprintf(listing, "[\n"); break;
    case RBRACKET:fprintf(listing, "]\n"); break;
    case LBRACE:fprintf(listing, "{\n"); break;
    case RBRACE:fprintf(listing, "}\n"); break;
    case LNOTE:fprintf(listing, "/*\n"); break;
    case RNOTE:fprintf(listing, "*/\n"); break;
    case GE:fprintf(listing, ">=\n"); break;
    case LE:fprintf(listing, "<=\n"); break;
    case NUM:
        fprintf(listing, "NUM, val= %s\n", tokenString);
        break;
    case ID:
        fprintf(listing, "ID, name= %s\n", tokenString);
        break;
    case ERROR_INP:
        fprintf(listing, "ERROR: %s\n", tokenString);
        break;
    default: /* should never happen */
        fprintf(listing, "Unknown token: %d\n", token);
    }
}


/*函数功能说明:为语法树创建一个语句节点  */
/*输入接口信息:StmtKind类型:语句节点的语句类型  */
/*输出接口信息:TreeNode类型的节点  */
/*调用注意事项:  */
TreeNode * newStmtNode(StmtKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
    }
    return t;
}

/*函数功能说明:创建用于语法树构造的表达式节点  */
/*输入接口信息:ExpKind类型:表达式节点的类型  */
/*输出接口信息:TreeNode类型的表达式节点地址  */
/*调用注意事项:  */
TreeNode * newExpNode(ExpKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
    }
    return t;
}

/*函数功能说明:分配并生成现有字符串的新副本   */
/*输入接口信息:char *类型的数据  */
/*输出接口信息:char *类型的数据  */
/*调用注意事项:  */
char * copyString(char * s)
{
    int n;
    char * t;
    if (s == NULL) return NULL;
    n = strlen(s) + 1;
    t = (char *)malloc(n);
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else strcpy(t, s);
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/*函数功能说明:按打印空间缩进   */
/*输入接口信息:void  */
/*输出接口信息:void  */
/*调用注意事项:   */
static void printSpaces(void)
{
    int i;
    for (i = 0; i < indentno; i++)
        fprintf(listing, " ");
}

/*函数功能说明:打印语法树   */
/*输入接口信息:TreeNode *:表示需要打印的语法树节点  */
/*输出接口信息:void  */
/*调用注意事项:  */
void printTree(TreeNode * tree)
{
    int i;
    INDENT;
    while (tree != NULL)
    {
        printSpaces();
        if (tree->nodekind == StmtK)
        {
            switch (tree->kind.stmt)
            {
            case IfK:
                fprintf(listing, "If\n");
                break;
            case DoK:
                fprintf(listing, "Do\n");
                break;
            case WhileK:
                fprintf(listing, "While\n");
                break;
            case ElseK:
                fprintf(listing, "Else\n");
                break;
            case ExpStmtK:
                fprintf(listing, "Expression\n");
                break;
            case AssignK:
                switch (tree->type)
                {
                case ArrayUnit:
                    fprintf(listing, "Assign to: %s[]\n", tree->attr.name); break;
                default:
                    fprintf(listing, "Assign to: %s\n", tree->attr.name); break;
                }
                break;
            case ReturnK:
                fprintf(listing, "return\n");
                break;
            case FunctionK:
                fprintf(listing, "Function Definition: ");
                switch (tree->type)
                {
                case Integer:
                    fprintf(listing, "Return Type: %s Name: %s\n", "[int]", tree->attr.name);
                    break;
                case FloaT:
                    fprintf(listing, "Return Type: %s Name: %s\n", "[float]", tree->attr.name);
                    break;
                case Void:
                    fprintf(listing, "Return Type %s Name: %s \n", "[void]", tree->attr.name); break;
                default:break;
                }
                break;
            case ParamK:
                switch (tree->type)
                {
                case Integer:
                    fprintf(listing, "Type: %s Name: %s \n", "[int]", tree->attr.name); break;
                case FloaT:
                    fprintf(listing, "Type: %s Name: %s \n", "[float]", tree->attr.name); break;
                case IntArray:
                    fprintf(listing, "Type: %s Name: %s \n","[int*]", tree->attr.name); break;
                case FloatArray:
                    fprintf(listing, "Type: %s Name: %s \n","[float*]", tree->attr.name); break;
                case Void:
                    fprintf(listing, "EMPTY\n"); break;
                }
                break;
            case VarDefinitionK:
                fprintf(listing, "Var Definition: ");
                switch (tree->type)
                {
                case Integer:
                    fprintf(listing, "Name: %s Type: %s \n", "[int]",tree->attr.name); break;
                case FloaT:
                    fprintf(listing, "Type: %s Name: %s \n", "[float]", tree->attr.name); break;
                case IntArray:
                    fprintf(listing, "Name: %s Type: %s \n", "[int*]",tree->attr.name); break;
                case FloatArray:
                    fprintf(listing, "Name: %s Type: %s \n","[float*]", tree->attr.name); break;
                //case Void:
                    //fprintf(listing, "Type %s :\n", "[void]"); break;
                }
                break;
            case LocalDefinitionK:
                fprintf(listing, "Local Definitions:\n");
                break;
            case StmtListK:
                fprintf(listing, "StatementList:\n");
                break;
            case ComK:
                fprintf(listing, "Compound:\n");
                break;
            case CallK:
                fprintf(listing, "Call Function:%s()\n", tree->attr.name);
                break;
            case ArgK:
                fprintf(listing, "Args:\n");
                break;
            case ParamListK:
                fprintf(listing, "ParamList:\n");
                break;
            case ArgListK:
                fprintf(listing, "ArgList:\n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp)
            {
            case OpK:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstK:
                if(tree->attr.val-int(tree->attr.val)==0)
                {
                    fprintf(listing, "const: %d\n",int(tree->attr.val));
                    break;
                }
                fprintf(listing, "const: %g\n", tree->attr.val);
                break;
            case IdK:
                switch (tree->type)
                {
                case ArrayUnit:
                    fprintf(listing, "Id: %s[]\n", tree->attr.name); break;
                default:
                    fprintf(listing, "Id: %s\n", tree->attr.name); break;
                }
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        }
        else
            fprintf(listing, "Unknown node kind\n");
        for (i = 0; i < MAXCHILDREN; i++)
        {
            if (tree->child[i] != NULL)
                printTree(tree->child[i]);
        }
        tree = tree->sibling;
    }
    UNINDENT;
}
