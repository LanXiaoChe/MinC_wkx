#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include<QFileDialog>
#include<QTextStream>
#include"analyse.h"
#include"scan.h"
using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

char prog[1000];//用来保存文件中的所有字符
char ch, ch1;//用来保存当前字符
char token[1000];//用来保存一个完整的字符
int p = 0;//文件中所有字符的下标
int sym = 0, n, line = 1;//状态标识符
string ch_line="";//处理后的字符串合集
const char* keyword[8] = { "if","else","int","float","return","void","do","while"};

void printToken(char* prog)
{
    sym = 0;//初始状态为0

    //先把token[]数组清空
    for (n = 0; n < 1000; n++)
    {
        token[n] = '\0';
    }

    n = 0;
    ch = prog[p++];//获取当前字符
    ch1 = prog[p];

    //如果当前字符是空格则跳过空格，tab的识别
    while (ch == ' ' || ch == '\t')
    {
        ch = prog[p++];//如果是空格就一直往下找
    }
    if (ch == '\n')
    {
        line++;//行数
        return;
    }

    if(ch=='_'&&(ch1==';'))
    {

            token[0]=ch;
            ch_line=ch_line+std::to_string(line)+"    下划线     "+token+'\n';
            return;

    }
    //以字母或者_开头即为标识符或关键字

     if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||(ch=='_'))
     {
        //标识符
       sym=1;
            do
            {
                token[n++] = ch;
                ch = prog[p++];

            } while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')||(ch=='_'));
        //比较标识符与关键字是否相同，若相同转为状态2
        for (n = 0; n < 8; n++)
        {
            if (strcmp(token, keyword[n]) == 0)
            {
                sym = 2;
                break;
            }
        }
        p--;
        if(sym==1)
        {
            ch_line=ch_line+std::to_string(line)+"    标识符     "+token+'\n';
        }
        else if(sym==2)
        {
            ch_line=ch_line+std::to_string(line)+"    关键字     "+token+'\n';
        }
        return;
       }


    //若以数字开头即为整数或浮点数
    if (ch >= '0' && ch <= '9')
    {
        //识别到数字，置状态为3
        sym = 3;
        do
        {
            token[n++] = ch;
            ch = prog[p++];
            //如果遇到.则为浮点数
            if (ch == '.')
            {
                sym = 31;
                token[n++] = ch;
                ch = prog[p++];
            }

        } while (ch >= '0' && ch <= '9');
        p--;
        if(sym==3)
        {
            ch_line=ch_line+std::to_string(line)+"    数字（整型）  "+token+'\n';
        }
        else if(sym==31)
        {
            ch_line=ch_line+std::to_string(line)+"    数字（浮点数） "+token+'\n';
        }

        return;
    }

     //该部分是注释/**/
    else if (ch == '/' && ch1 == '*')
    {
        do
        {
            token[n++] = ch;
            ch = prog[p++];
            ch1=prog[p];
            if (ch == '\n')
            {
                line++;
            }

        } while (ch != '*' || ch1 != '/');

        p++;


        return;
    }

    //运算符
    else if ((ch == '<'||ch=='>'||ch=='='))
    {
        if(ch1=='=')
        {
            p++;
            token[0] = ch;
            token[1] = ch1;
            if(token[0]=='<')
            {
                ch_line=ch_line+std::to_string(line)+"    小于等于号   "+token+'\n';
            }
            else if(token[0]=='>')
            {
                ch_line=ch_line+std::to_string(line)+"    大于等于号   "+token+'\n';
            }
            else if(token[0]=='=')
            {
                ch_line=ch_line+std::to_string(line)+"    等于号     "+token+'\n';
            }

        }
        else
        {
            token[0]=ch;
            if(token[0]=='=')
            {
                ch_line=ch_line+std::to_string(line)+"    赋值符号    "+token+'\n';
            }
            else if(token[0]=='<')
            {
                ch_line=ch_line+std::to_string(line)+"    小于号     "+token+'\n';
            }
            else if(token[0]=='>')
            {
                ch_line=ch_line+std::to_string(line)+"    大于号     "+token+'\n';
            }
        }
        return;
    }

    //运算符
    else if (ch=='!'&&ch1=='=')
    {
        p++;
        token[0] = ch;
        token[1] = ch1;
        ch_line=ch_line+std::to_string(line)+"    不等号     "+token+'\n';
        return;
    }



    //运算符号
    else if(ch=='*'||ch=='%'||ch=='/'||ch=='+'||ch=='-')
    {

        token[0]=ch;

         if(token[0]=='*')
        {
            ch_line=ch_line+std::to_string(line)+"    乘号      "+token+'\n';
        }
        else if(token[0]=='%')
        {
            ch_line=ch_line+std::to_string(line)+"    求余号     "+token+'\n';
        }
        else if(token[0]=='/')
        {
            ch_line=ch_line+std::to_string(line)+"    除号      "+token+'\n';
        }
        else if(token[0]=='+')
        {
            ch_line=ch_line+std::to_string(line)+"    加号      "+token+'\n';
        }
        else if(token[0]=='-')
        {
            ch_line=ch_line+std::to_string(line)+"    减号      "+token+'\n';
        }
        return;

    }

    //特殊符号即分界符号
    else
    {
    switch (ch)//识别关键符号
        {
        case '{': ch_line=ch_line+std::to_string(line)+"    左花括号    "+ch+'\n';break;
        case '}':ch_line=ch_line+std::to_string(line)+"    右花括号    "+ch+'\n';break;
        case ';':ch_line=ch_line+std::to_string(line)+"    分号      "+ch+'\n';break;
        case '(':ch_line=ch_line+std::to_string(line)+"    左括号     "+ch+'\n';break;
        case ')':ch_line=ch_line+std::to_string(line)+"    右括号     "+ch+'\n';break;
        case ',':ch_line=ch_line+std::to_string(line)+"    逗号      "+ch+'\n';break;
        case '[': ch_line=ch_line+std::to_string(line)+"    左中括号    "+ch+'\n';break;
        case ']': ch_line=ch_line+std::to_string(line)+"    左中括号    "+ch+'\n';break;
        default:sym = -1; break;

        }
    return;
    }
}


/*函数功能说明:进行词法分析*/
/*输入接口信息:char* prog是用来保存文件中的所有字符  */
/*输出接口信息:void*/
/*调用注意事项:*/


void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                            this, tr("open image file"),
                            "./", tr("List files(*.txt *.php *.dpl *.m3u *.m3u8 *.xspf );;All files (*.*)"));

    if(fileName.isEmpty())
    {
         ui->textEdit->setText("用户取消操作！！");
    }

    else
    {
        ui->lineEdit->setText(fileName);//显示文件路径
        QFile f(fileName);
        if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
             ui->textEdit->setText("Fail！！");
             return;
        }
        QTextStream txtInput(&f);
        txtInput.setCodec("utf-8");
        QString lineStr;
        while(!txtInput.atEnd())
        {
            lineStr = lineStr + txtInput.readLine() + "\n";
        }
        ui->textEdit->setText(lineStr);//显示txt文件内容
        f.close();
    }
}

void MainWindow::on_yufa_clicked()
{
    QString text = ui->textEdit->toPlainText();
    if(text == nullptr)
    {
        QMessageBox::information(this,"error","内容为空！");
        return;
    }
    QFile tempFile(input);
    tempFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&tempFile);
    out<<ui->textEdit->toPlainText();
    tempFile.close();
    string s_input = input.toStdString();
    string s_output = output.toStdString();
    analyse(s_input,s_output);
    QMessageBox::information(this,"Message","生成成功，点击查看进行查看语法树！");
}

void MainWindow::on_yufa_show_clicked()
{
    QFile *file = new QFile;
        file->setFileName(output);
        bool ok = file->open(QIODevice::ReadOnly);
        if(ok)
        {
            QTextStream in(file);
            ui->textEdit_3->setText(in.readAll());
            file->close();
            delete file;
        }
}

void MainWindow::on_cifa_clicked()
{
    ch_line="";
    QString s=ui->textEdit->toPlainText();
    string uu=s.toStdString();
    ch_line=ch_line+"行号  "+"类型     "+"符号"+'\n';
    int k=0;
    for(int i=0;i<=uu.length();i++)
        {
                prog[k++]=char(uu[i]);
        }
        int m=0;
        do
        {
           printToken(prog);
            m++;
        }while(m!=k);

        s=QString::fromStdString(ch_line);
        ui->textEdit_2->setText(s);
}

void MainWindow::on_cifa_save_clicked()
{
    QString text = ui->textEdit_2->toPlainText();
        if(text == nullptr)
        {
            QMessageBox::information(this,"error","内容为空！");
            return;
        }
        QString fileName1 = QFileDialog::getSaveFileName(this,tr("Savetree"),"新建文本",tr("txt文件(*.txt)"));
        QFile sourceFile(fileName1);
        if(!sourceFile.open(QFile::WriteOnly | QFile::Text)) //检测文件是否打开
        {
            QMessageBox::information(this,"Error Message", "请选择一个文件！");
            return ;
        }
        QTextStream out(&sourceFile);
        out<<ui->textEdit_2->toPlainText();
        QMessageBox::information(this,"Message","保存成功！");
        sourceFile.close();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString text = ui->textEdit->toPlainText();
    if(text == nullptr)
    {
        QMessageBox::information(this,"error","内容为空！");
        return;
    }
    QFile tempFile(input);
    tempFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&tempFile);
    out<<ui->textEdit->toPlainText();
    tempFile.close();
    string s_input = input.toStdString();
    string s_output = output_yuyi.toStdString();

    analyse_yuyi(s_input,s_output);
    QMessageBox::information(this,"Message","生成成功");

    QFile *file = new QFile;
        file->setFileName(output_yuyi);
        bool ok = file->open(QIODevice::ReadOnly);
        if(ok)
        {
            QTextStream in(file);
            ui->textEdit_4->setText(in.readAll());
            file->close();
            delete file;
        }


}


void MainWindow::on_pushButton_3_clicked()
{
    QString text = ui->textEdit->toPlainText();
    if(text == nullptr)
    {
        QMessageBox::information(this,"error","内容为空！");
        return;
    }
    QFile tempFile(input);
    tempFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream out(&tempFile);
    out<<ui->textEdit->toPlainText();
    tempFile.close();
    string s_input = input.toStdString();
    string s_output = output_code.toStdString();
    analyse_code(s_input,s_output);
    QMessageBox::information(this,"Message","生成成功");

    QFile *file = new QFile;
        file->setFileName(output_code);
        bool ok = file->open(QIODevice::ReadOnly);
        if(ok)
        {
            QTextStream in(file);
            ui->textEdit_5->setText(in.readAll());
            file->close();
            delete file;
        }
}
