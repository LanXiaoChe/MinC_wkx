/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:scan.cpp                          */
/* 模块功能说明:词法分析获取token                    */
/* 当前版本号:1.0.1                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.19                            */
/*************************************************/


#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
    START, INEQUAL, INNEQ, INCOMMENT, INNUM, INID, DONE, INLT, INRT
}
StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

char lineBuf[BUFLEN]; /* holds the current line */
int linepos = 0; /* current position in LineBuf */
int bufsize = 0; /* current size of buffer string */
int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */


/*函数功能说明:从lineBuf获取下一个非空字符，如果lineBuf耗尽，则读取新行*/
/*输入接口信息:void                                 */
/*输出接口信息:转化为int类型的ASCII码            */
/*调用注意事项:                 */
static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source))
        {
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++];
        }
        else
        {
            EOF_flag = TRUE;
            return EOF;
        }
    }
    else return lineBuf[linepos++];
}

/*函数功能说明:在lineBuf中回溯一个字符    */
/*输入接口信息:void                                 */
/*输出接口信息:void            */
/*调用注意事项:                 */
static void ungetNextChar(void)
{
    if (!EOF_flag) linepos--;
}

/* lookup table of reserved words */
static struct
{
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED] = { { "if",IF },{ "else",ELSE },{ "int",Int },
{ "void",VoiD },{ "return",RETURN },{ "while",WHILE },{"do",Do},{"float",Float} };

/*函数功能说明:使用线性搜索查找标识符以查看它是否是保留字    */
/*输入接口信息:char *类型,表示需要搜索的标识符                         */
/*输出接口信息:TokenType类型的值            */
/*调用注意事项:                 */
static TokenType reservedLookup(char * s)
{
    int i;
    for (i = 0; i < MAXRESERVED; i++)
        if (!strcmp(s, reservedWords[i].str))
            return reservedWords[i].tok;
    return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/

/*函数功能说明:返回源文件中的下一个标记    */
/*输入接口信息:void                                    */
/*输出接口信息:TokenType类型的值            */
/*调用注意事项:                  */
TokenType getToken(void)
{
   /* index for storing into tokenString */
    int tokenStringIndex = 0;
    /* holds current token to be returned */
    TokenType currentToken;
    /* current state - always begins at START */
    StateType state = START;
    /* flag to indicate save to tokenString */
    int save;
    int o=0;
    while (state != DONE)
    {
        int c = getNextChar();
        save = TRUE;
        switch (state)
        {
        case START://开头的标志
            if (isdigit(c))
                state = INNUM;
            else if (isalpha(c)||c=='_')
            {
                state = INID;
                o=o+1;
            }
            else if (c == '=')
                state = INEQUAL;
            else if (c == '<')
                state = INLT;
            else if (c == '>')
                state = INRT;
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = FALSE;
            else if (c == '!')
                state = INNEQ;
            else if (c == '/')
            {
                if (getNextChar() != '*')
                {
                    ungetNextChar();
                    state = DONE;
                    currentToken = OVER;
                    break;
                }
                else
                {
                    save = FALSE;
                    state = INCOMMENT;
                }
            }
            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case '%':
                    currentToken = MOOD;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case '[':
                    currentToken = LBRACKET;
                    break;
                case ']':
                    currentToken = RBRACKET;
                    break;
                case '{':
                    currentToken = LBRACE;
                    break;
                case '}':
                    currentToken = RBRACE;
                    break;
                case ',':
                    currentToken = DOT;
                    break;
                default:
                    currentToken = ERROR_INP;
                    break;
                }
            }
            break;
        case INCOMMENT:
            save = FALSE;
            if (c == EOF)
            {
                state = DONE;
                currentToken = ERROR_INP;
            }
            else if (c == '*')
            {
                if (getNextChar() == '/')
                    state = START;
                else
                    ungetNextChar();
            }
            break;
        case INNEQ:
            state = DONE;
            if (c == '=')
                currentToken = NEQ;
            else
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR_INP;
            }
            break;
        case INEQUAL:
            state = DONE;
            if (c == '=')
                currentToken = EQ;
            else
            {
                ungetNextChar();
                currentToken = ASSIGN;
            }
            break;
        case INNUM:
            if (isdigit(c)||(c=='.'))
            {

            }
            else
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID://ID
            if (!isalpha(c)&&c!='_'&&!isdigit(c))
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            else
            {
                o=o+1;
            }
            break;
        case INLT:
            state = DONE;
            if (c == '=')
                currentToken = LE;
            else
            {
                ungetNextChar();
                currentToken = LT;
            }
            break;
        case INRT:
            state = DONE;
            if (c == '=')
                currentToken = GE;
            else
            {
                ungetNextChar();
                currentToken = RT;
            }
            break;
        case DONE:
        default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state);
            state = DONE;
            currentToken = ERROR_INP;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))
        {

            tokenString[tokenStringIndex++] = (char)c;
        }
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if(o==1&&c=='_')
            {
                currentToken = UNLINE;
            }
            if (currentToken == ID)
            {
                currentToken = reservedLookup(tokenString);
            }
        }
    }
    if (TraceScan)
    {

        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
}

