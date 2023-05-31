/************************************************/
/* 版权信息:2022 20183231114王可心                */
/* 文件名称:parse.cpp                          */
/* 模块功能说明:语法分析使用递归下降分析语法        */
/* 当前版本号:1.0.2                               */
/* 作者:王可心                                    */
/* 完成日期: 2022.3.22                           */
/*************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"
char *rt[1000][1000];
int r;
static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * program(void);
static TreeNode * definition_list(void);
static TreeNode * definition(void);
static TreeNode * params(void);
static TreeNode * param_list(void);
static TreeNode * param(void);
static TreeNode * compound_stmt(void);
static TreeNode * local_definitions(void);
static TreeNode * statement_list(void);
static TreeNode * statement(void);
static TreeNode * expression_stmt(void);
static TreeNode * if_stmt(void);
static TreeNode * Dowhile_stmt(void);
static TreeNode * return_stmt(void);
static TreeNode * expression(void);
static TreeNode * var(void);
static TreeNode * simple_exp(void);
static TreeNode * additive_expression(void);
static TreeNode * term(void);
static TreeNode * factor(void);
static TreeNode * args(void);
static TreeNode * arg_list(void);

/*函数功能说明:显示词法错误所在行和词*/
/*输入接口信息:char *类型表示错误的词 */
/*输出接口信息:void                */
/*调用注意事项:                    */
static void syntaxError(char * message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

/*函数功能说明:判断读取的词是否为预计的词，是则获取下一词；否则调用syntaxError报错*/
/*输入接口信息:TokenTypeInProjece:预计读取的字符类型  */
/*输出接口信息:void                    */
/*调用注意事项:                 */
static void match(TokenType expected)
{
    if (token == expected) token = getToken();
    else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

/*函数功能说明:program->definition_list  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项: */
TreeNode * program(void)
{
    TreeNode * t=NULL;
    t = definition_list();//program->definition_list
     return t;
}

/*函数功能说明:definition_list->definition{@|definition}  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * definition_list(void)
{
    TreeNode * t = definition();
    TreeNode * p = t;
    while ((token == Int) || (token == VoiD)||(token == Float))//definition_list->definition{@|definition}
    {
        TreeNode *q = definition();
        if (q != NULL)
        {
            if (t == NULL)
                t = p = q;
            else
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

/*函数功能说明:definition->variable_definition | function_definition  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:注意返回值的数据类型                   */
char* type_hai;
int ti;
TreeNode * definition(void)
{
    TreeNode * t = NULL;
    type_hai="";
    switch (token)
    {
    case VoiD:
    case Float:
    case Int:
        t = newStmtNode(DefinitionK);
        if (token == Int)
        {t->type = Integer;
            type_hai="Integer";
        }
        else if(token==Float)
           { t->type = FloaT;
             type_hai="FloaT";
        }
        else
        {t->type = Void;
            type_hai="Void";
        }
        match(token);
        switch (token)
        {
        case ID://variable_definition->type_indicator ID; | type_indicator ID[NUM]; | type_indicator ID(parameters) compound_stmt
            t->attr.name = copyString(tokenString);
            t->kind.stmt = VarDefinitionK;
            match(ID);
            switch (token)
            {
            case LBRACKET://variable_definition->type_indicator ID[NUM];

               t->kind.stmt = VarDefinitionK;
                if(t->type==Integer)
                    t->type = IntArray;
                else if(t->type==FloaT)
                    t->type = FloatArray;
                match(LBRACKET);
                t->child[0]=newExpNode(ConstK);
                if ((t->child[0] != NULL) && (token == NUM))
                {

                    t->child[0]->attr.val = atof(tokenString);

                }
                if(t->child[0]->attr.val-int(t->child[0]->attr.val)==0)
                {
                    t->child[0]->type=Integer;
                }
                else
                    t->child[0]->type=FloaT;
                match(NUM);
                match(RBRACKET);
                match(SEMI);
                break;
            case LPAREN://function_definition->type_indicator ID(parameters) compound_stmt
               t->kind.stmt = FunctionK;
               rt[r++][0]=t->attr.name;
                match(LPAREN);
                ti=0;
                t->child[0] = params();
                match(RPAREN);
                t->child[1] = compound_stmt();
                break;
            default://进入variable_definition->type_indicator ID;
                match(SEMI);
                break;
            }
            break;
        default:
            syntaxError("unexpected token -> ");
            printToken(token, tokenString);
            token = getToken();
            break;
        }
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}
/*函数功能说明:parameters->parameter_list | void  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * params(void)
{
    TreeNode * t = NULL;
    if (token == VoiD)//parameters->void
    {
        match(token);
        TreeNode * t = newStmtNode(ParamListK);
        t->child[0] = newStmtNode(ParamK);
        t->child[0]->type = Void;
    }
    else if (token == RPAREN)//function_definition->type_indicator ID(parameters) compound_stmt
        t = NULL;
    else//进入parameters->parameter_list
    {
        t = param_list();
    }
    return t;
}

/*函数功能说明:parameter_list->parameter{,parameter}  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * param_list(void)
{
    TreeNode * t = newStmtNode(ParamListK);
    int i = 1;
    ti=1;
    t->child[0] = param();
    while (token != RPAREN)//parameter_list->parameter{,parameter}
    {
        match(DOT);
        ti=i+1;
        t->child[i] = param();
        i++;
    }
    return t;
}

/*函数功能说明:parameter->type_indicator ID | type_indicator ID[]  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * param(void)
{
    TreeNode * t = NULL;//parameter->type_indicator ID

    if(token==Int)
    {
        match(Int);
        t = newStmtNode(ParamK);
        rt[r-1][ti]="[int]";
        t->type = Integer;
        t->attr.name = copyString(tokenString);
    }
    else if(token==Float)
    {
        match(Float);
        t = newStmtNode(ParamK);
        rt[r-1][ti]="[float]";
        t->type = FloaT;
        t->attr.name = copyString(tokenString);
    }

    match(ID);
    if (token == LBRACKET)//parameter->type_indicator ID[]
    {
        if(t->type==Integer)
        {
             t->type = IntArray;
             rt[r-1][ti]="[int*]";
        }
        else if(t->type==FloaT)
        {
             t->type = FloatArray;
             rt[r-1][ti]="[float*]";
        }
        match(LBRACKET);
        match(RBRACKET);
    }
    return t;
}

/*函数功能说明:compound_stmt->{local_definitions statement_list}  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项: */
TreeNode * compound_stmt(void)
{
    TreeNode * t = newStmtNode(ComK);
    match(LBRACE);
    t->child[0] = local_definitions();
    t->child[1] = statement_list();
    match(RBRACE);
    return t;
}

/*函数功能说明:local_definitions-> { variable_definition }  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * local_definitions(void)
{
    TreeNode * t = newStmtNode(LocalDefinitionK);
    int i = 0;
    while (token == Int || token == VoiD || token == Float)//local_definitions->variable_definition
    {

        t->child[i] = definition();
        i++;
    }//local_definitions->@
    return t;
}

/*函数功能说明:statement_list-> { statement }  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * statement_list(void)
{
    TreeNode * t = newStmtNode(StmtListK);
    int i = 0;
    while (token != RBRACE)//statement_list->statement
    {
        t->child[i] = statement();
        i++;
    }//statement_list->@
    return t;
}

/*函数功能说明:statement-> expression_stmt | compound_stmt | confition_stmt | dowhile_stmt | return_stmt  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * statement(void)
{
    TreeNode * t = NULL;
    switch (token)
    {
    case IF:
        t = if_stmt(); break;//statement-> confition_stmt
    case Do:
        t = Dowhile_stmt(); break;//statement->dowhile_stmt
    case ID:
    case SEMI:
        t = expression_stmt(); break;//statement-> expression_stmt
    case RETURN:
        t = return_stmt(); break;//statement-> return_stmt
    case LBRACE:
        t = compound_stmt(); break;//statement->compound_stmt
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*函数功能说明:expression_stmt-> expression; | ;  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * expression_stmt(void)
{
    TreeNode * t = newStmtNode(ExpStmtK);
    if (token == SEMI)//expression_stmt-> ;
        match(SEMI);
    else//expression_stmt->expression;
    {
        t = expression();
        match(SEMI);
    }
    return t;
}

/*函数功能说明:condition_stmt->if (expression) statement [else statement]   */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * if_stmt(void)
{
    TreeNode * t = newStmtNode(IfK);
    if (t != NULL)
    {
        match(IF);//condition_stmt-> if (expression) statement
        match(LPAREN);
        t->child[0] = expression();
        match(RPAREN);
        t->child[1] = statement();
        if (token == ELSE)//condition_stmt-> if (expression) statement else statement
        {
            match(ELSE);
            if (t != NULL)
                t->child[2] = newStmtNode(ElseK);
            t->child[2]->child[0] = statement();
        }
    }
    return t;
}

/*函数功能说明:dowhile_stmt -> do statement while (expression);*/
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * Dowhile_stmt(void)
{
    TreeNode* t = newStmtNode(DoK);
    match(Do);
    if (t != NULL) t->child[0] = statement();
    match(WHILE);

    match(LPAREN);
    if (t != NULL)
    {t->child[1]=newStmtNode(WhileK);t->child[1]->child[0]= expression();}
    match(RPAREN);
    match(SEMI);
    return t;
}

/*函数功能说明:return_stmt-> return ; | return expression ;  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:注意返回值的数据类型                   */
TreeNode * return_stmt(void)
{
    TreeNode * t = newStmtNode(ReturnK);
    t->attr.name="return";
   if(strcmp(type_hai,"Integer")==0)
   {
       t->type=Integer;
   }
   else if(strcmp(type_hai,"FloaT")==0)
   {
       t->type=FloaT;
   }
   else if(strcmp(type_hai,"Void")==0)
   {
       t->type=Void;
   }
    if (token == RETURN)
        match(RETURN);
    if (token == SEMI)//return_stmt-> return ;
        match(SEMI);
    else//return_stmt-> return expression ;
    {
        t->child[0] = expression();
        match(SEMI);
    }
    return t;
}

/*函数功能说明:expression-> simple_expression  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode * expression(void)
{
    TreeNode * t = simple_exp();
    return t;
}

/*函数功能说明:variable->ID [expression]  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode* var(void)
{
    TreeNode* t = newExpNode(IdK);
    if ((t != NULL) && (token == ID))//variable-> ID
        t->attr.name = copyString(tokenString);
    match(ID);
    if (token == LBRACKET)//variable->ID[expression]
    {
        match(token);
        t->type = ArrayUnit;
        t->child[0] = expression();
        match(RBRACKET);
    }
    return t;
}

/*函数功能说明:simple_exp->additive_expression[relop additive_expresison]*/
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项: */
TreeNode * simple_exp(void)
{
    TreeNode * t = additive_expression();
    if (t != NULL)
    {
        if (token == LT || token == LE || token == RT || token == GE || token == EQ || token == NEQ)
        {
            TreeNode * p = newExpNode(OpK);
            if (p != NULL)
            {
                p->attr.op = token;
                p->child[0] = t;
                match(token);
                p->child[1] = additive_expression();
                t = p;
            }
        }
    }
    return t;
}

/*函数功能说明:additive_expression->term{addop term}*/
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点                   */
/*调用注意事项:*/
TreeNode* additive_expression(void)
{
    TreeNode * t = term();
    while (token == PLUS || token == MINUS)//additive_expression->term addop term
    {
        TreeNode * p = newExpNode(OpK);
        p->attr.op = token;
        p->child[0] = t;
        match(token);
        p->child[1] = term();
        t = p;
    }//additive_expression->term
    return t;
}

/*函数功能说明:term->factor{mulop factor}  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点*/
/*调用注意事项:*/
TreeNode * term(void)
{
    TreeNode * t = factor();
    while ((token == TIMES) || (token == OVER)||(token==MOOD))//term->factor mulop factor
    {
        TreeNode * p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            match(token);
            p->child[1] = factor();
            t = p;
        }
    }//term->factor
    return t;
}
/*函数功能说明:factor->(expression)|variable|call|NUM  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点*/
/*调用注意事项: */
TreeNode * factor(void)
{
    TreeNode * t = NULL;
    switch (token)
    {
    case NUM://进入factor->NUM
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
        {

            t->attr.val = atof(tokenString);

        }

        if(t->attr.val-int(t->attr.val)==0)
        {
            t->type=Integer;
        }
        else
            t->type=FloaT;
        match(NUM);
        break;
    case ID://进入factor->variable||factor->call
        t = var();
        if (token == ASSIGN)//expression->variable=expression
        {
            TreeNode* p = newStmtNode(AssignK);
            p->attr.name = t->attr.name;
            match(token);
            p->child[0] = expression();
            t = p;
        }
        if (token == LPAREN)//(call-> ID(arguments)
        {
            TreeNode * p = newStmtNode(CallK);
            p->attr.name = t->attr.name;
            match(token);
            p->child[0] = args();
            t = p;
            if(strcmp(t->attr.name,"output")==0)
            {
                    t->type=Void;
                    t->attr.name="output";
            }
            else if(strcmp(t->attr.name,"input")==0)
            {
                    t->type=Integer;
                    t->attr.name="input";
            }

            match(RPAREN);
        }
        break;
    case LPAREN://factor->(expression)
        match(LPAREN);
        t = expression();
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/*函数功能说明:arguments->{argument_list}  */
/*输入接口信息:void */
/*输出接口信息:TreeNode*类型的语法树节点*/
/*调用注意事项:*/
TreeNode * args(void)
{
    TreeNode * t = newStmtNode(ArgK);
    if (token != RPAREN)//arguments->argument_list
    {
        t->child[0] = arg_list();
        return t;
    }
    else
        return NULL;
}

/*函数功能说明:argument_list->expression{,expression}  */
/*输入接口信息:void  */
/*输出接口信息:TreeNode*类型的语法树节点*/
/*调用注意事项:*/
TreeNode * arg_list(void)
{
    TreeNode * t = newStmtNode(ArgListK);
    int i = 0;
    if (token != RPAREN)//argument_list->expression
        t->child[i] = expression();
    i++;
    while (token != RPAREN)//argument_list->expression{,expression }
    {
        match(DOT);
        t->child[i] = expression();
        i++;
    }
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/*函数功能说明:返回新的构造的整个语法树    */
/*输入接口信息:void                                 */
/*输出接口信息:TreeNode*类型的语法树节点            */
/*调用注意事项:               */
TreeNode * parse(void)
{
    TreeNode * t;
    for(int i=0;i<1000;i++)
    {
        for(int j=0;j<1000;j++)
        {
            rt[i][j]="@";
        }
    }
    r=0;
    token = getToken();
    t = program();
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}

char * rtchange(int a,int b)
{
    return rt[a][b];
}
int  rtchange1(){
    return r;
}
