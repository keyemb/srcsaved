#include "form.h"
#include "ui_form.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QTextCodec>

//QString File_of_Kaoqin = "/home/sd/";

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

Form::~Form()
{
    delete ui;
}


void Form::showForm(QStringList str1, QStringList str2)
{
    this->show();
    ui->lineEdit_classtime->setText(str1.at(0));
    ui->lineEdit_classname->setText(str1.at(1));
    ui->lineEdit_teacher->setText(str1.at(2));
    ui->lineEdit_class->setText(str1.at(3));
    ui->lineEdit_stunum->setText(str1.at(4));
    ui->lineEdit_stunumtrue->setText(str1.at(5));

    for(int i=0;i<str2.length();i++)
    {
        ui->textBrowser->insertPlainText(str2.at(i));
    }
}


void Form::on_pushButton_2_clicked()
{
    this->close();
}

void Form::on_pushButton_clicked()
{
    //QString File_path = File_of_Kaoqin.trimmed().append(ui->lineEdit_classtime->text().trimmed().append(ui->lineEdit_class->text().trimmed().append(ui->lineEdit_classname->text().trimmed().append(".txt"))));
    //qDebug()<<File_path<<endl;
    //File_path = File_path.trimmed();
    //QString File_path = File_of_Kaoqin.append("啦.txt");

    QDateTime dt;
    QTime time;
    QDate date;
    dt.setTime(time.currentTime());
    dt.setDate(date.currentDate());
    //QString File_path = File_of_Kaoqin.append(dt.toString("yyyy-MM-dd-hh-mm-ss").append(".txt"));
    QFile f_class("/mnt/sd/kaoqin.txt");  //读取文件中的课程文件信息
    f_class.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    QTextStream file_class(&f_class);
    QTextCodec *codec=QTextCodec::codecForName("GB2312");
    file_class.setCodec(codec);
    file_class << tr("所在班级：") << ui->lineEdit_class->text() << "\n"
               << tr("课程时间：") << ui->lineEdit_classtime->text() << "\n"
               << tr("课程名称：") << ui->lineEdit_classname->text() << "\n"
               << tr("教师姓名：") << ui->lineEdit_teacher->text() << "\n"
               << tr("学生总数：") << ui->lineEdit_stunum->text() << "\n"
               << tr("实到人数：") << ui->lineEdit_stunumtrue->text() << "\n"
               << tr("缺课名单：") << ui->textBrowser->toPlainText() << "\n\n\n" <<endl;


    f_class.close();
    QMessageBox::warning(this, tr("提示！"),tr("已经将详细信息保存至本地文件中！！"));
}
