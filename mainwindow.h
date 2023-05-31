#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString output = "../syntaxTree.txt";
    QString input="../filename.txt";
    QString output_yuyi="../yuyifile.txt";
    QString output_code="../codefile.txt";

private slots:
    void on_pushButton_clicked();

    void on_yufa_clicked();

    void on_yufa_show_clicked();

    void on_cifa_clicked();

    void on_cifa_save_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
