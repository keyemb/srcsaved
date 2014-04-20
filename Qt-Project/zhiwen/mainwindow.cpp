#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"
//#include <QDebug>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
//#include <qfiledialog.h>
#include <QtSql/QSqlQuery>
//#include <QSqlError>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QFileInfo>

QByteArray temp1_n,temp2;
//QString codenu = "20110104";
QString File_Class = "/mnt/sd/class.txt";
QString File_Teacher = "/mnt/sd/teacher.txt";
QString File_db = "/mnt/sd/test.db";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    choose_sd_udisk();  //选择文件存放位置
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->label_photo->setAlignment(Qt::AlignCenter);
    ui->label_photo->setScaledContents(true);//照片显示剧中
    file_read_write();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //建立与数据库的链接
    db.setDatabaseName(File_db);
    if(!db.open())
    {
        QMessageBox::critical(0, tr("打开数据库失败！"),tr("不能和数据库建立链接！\n请检查SD卡是否插好？？"), QMessageBox::Cancel);
    }

    SetFlags(0); //所有标记清零

    QSqlQuery query;
    query.exec("select name from sqlite_master where type='table'");
    query.last();
    int num=query.at();
    query.first();

    QStringList strings;
    strings.append("");
    for(int i=0;i<num+1;i++)            //将已有数据显示到窗口中
    {
        //ui->textBrowser->append(query.value(0).toString());
        strings.append(query.value(0).toString());
        query.next();
    }
    ui->comboBox_class->addItems(strings);

    QString zhiwen1_n="f5 0c 00 00 00 00 0c f5";
    String2Hex(zhiwen1_n.toAscii().data(),temp1_n); //设置1：n对比质问命令


    serialinit();   //串口初始化

    myCom->write(temp1_n);
}

void MainWindow::serialinit()
{
    myCom = new Posix_QextSerialPort("/dev/tq2440_serial2",QextSerialBase::Polling);

    myCom ->open(QIODevice::ReadWrite);//以读写方式打开串口

    myCom->setBaudRate(BAUD19200);//波特率设置，我们设置为9600
    myCom->setDataBits(DATA_8);//数据位设置，我们设置为8位数据位
    myCom->setParity(PAR_NONE);//奇偶校验设置，我们设置为无校验
    myCom->setStopBits(STOP_1);//停止位设置，我们设置为1位停止位
    myCom->setFlowControl(FLOW_OFF);//数据流控制设置，我们设置为无数据流控制
    myCom->setTimeout(200);

    readTimer = new QTimer(this); //设置读取计时器
    readTimer->start(100);//设置延时为100ms
    connect(readTimer,SIGNAL(timeout()),this,SLOT(readMyCom()));//【linux下使用】信号和槽函数关联，当达到定时时间时，进行读串口操作
    //【以上三条与君不同于windows下的开发，非常重要】
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::file_read_write()
{
    if((!QFileInfo(File_Class).exists()) || (!QFileInfo(File_Teacher).exists()))
    {
        QMessageBox::warning(this, tr("警告！"),tr("课程信息文件及教师信息文件不存在！！\n请关闭系统，并添加相应文件！！"));
    }
    QFile f_class(File_Class);              //读取文件中的课程文件信息
    f_class.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream file_class(&f_class);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    file_class.setCodec(codec);
    QStringList str_class;
    str_class.append("");
    for(int i=0;!file_class.atEnd();i++)            //将已有数据显示到窗口中
    {
        str_class.append(file_class.readLine());
    }
    ui->comboBox_classname->addItems(str_class);
    f_class.close();

    QFile f_teacher(File_Teacher);              //读取文件中的课程文件信息
    f_teacher.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream file_teacher(&f_teacher);
    file_teacher.setCodec(codec);
    QStringList str_teacher;
    str_teacher.append("");
    for(int i=0;!file_teacher.atEnd();i++)            //将已有数据显示到窗口中
    {
        str_teacher.append(file_teacher.readLine());
    }
    ui->comboBox_teacher->addItems(str_teacher);
    f_teacher.close();
}

void MainWindow::ClearText()
{
    //ui->lineEdit_class->clear();
    ui->comboBox_class->setCurrentIndex(0);
    ui->comboBox_classname->setCurrentIndex(0);
    ui->comboBox_classtime->setCurrentIndex(0);
    ui->comboBox_teacher->setCurrentIndex(0);
    ui->lineEdit_grade->clear();
    ui->lineEdit_id->clear();
    ui->lineEdit_name->clear();
    ui->label_photo->clear();
}

void MainWindow::choose_sd_udisk()
{
    //QFileInfo file_db(File_db);

    if(!QFileInfo(File_db).exists())
    {
        File_db="/mnt/udisk/test.db";

        if(!QFileInfo(File_db).exists())
        {
            QMessageBox::question(NULL, tr("注意！"), tr("SD卡或U盘未插入或者数据库不存在，\n请插入SD卡或者U盘后点击YES\n或者检查数据库文件是否存在！"), QMessageBox::Ok, QMessageBox::Ok);
            File_db="/mnt/sd/test.db";
            choose_sd_udisk();
        }
        File_Class="/mnt/udisk/class.txt";
        File_Teacher="/mnt/udisk/teacher.txt";
    }
    if(File_db == "/mnt/udisk/test.db")
    {
        File_Class = "/mnt/sd/class.txt";
        File_Teacher = "/mnt/sd/teacher.txt";
    }

}

void MainWindow::readMyCom() //读取串口数据并显示出来
{
    temp2.append(myCom->readAll());
    ///QByteArray temp = myCom->readAll();
    //读取串口缓冲区的所有数据给临时变量temp
    //ui->textBrowser->insertPlainText(temp);
    //将串口的数据显示在窗口的文本浏览器中
    if(temp2.size() == 8)
    {
        int code = temp2[3] & 0xff;
        code |= (temp2[2] << 8) & 0xff00;
        if(code == 0)
        {
            //不存在此指纹，补充提示
            QMessageBox::warning(this, tr("警告！"),tr("指纹库中不存在此指纹！！"));
            temp2.clear();
            ui->lineEdit_grade->clear();
            ui->lineEdit_id->clear();
            ui->lineEdit_name->clear();
            ui->label_photo->clear();
            myCom->write(temp1_n);
            return;
        }
        //QString codes = codenu + QString::number(code,10);
        //ShowDate(codes);
        if(ui->comboBox_class->currentText() == "")
        {
            QMessageBox::warning(this, tr("警告！"),tr("请先选择班级名称！！"));
            myCom->write(temp1_n);
        }
        else
        {
            ShowDate(QString::number(code,10));
        }
        temp2.clear();
    }
}

void MainWindow::ShowDate(QString ids)
{
     QSqlQuery q;
     q.exec("SELECT * FROM '"+ui->comboBox_class->currentText()+"' WHERE id like '________"+ids+"'");
     SetFlags(ids);
     if(q.next())
     {
         if (q.isNull(0) == false)
         {
             ui->lineEdit_id->setText(q.value(0).toString());
             ui->lineEdit_name->setText(q.value(1).toString());
             ui->lineEdit_grade->setText(q.value(2).toString());
             //ui->lineEdit_class->setText(q.value(3).toString());
             QPixmap photo;
             photo.loadFromData(q.value(4).toByteArray());
             ui->label_photo->setPixmap(photo);
             myCom->write(temp1_n);
         }
         else
         {
             QMessageBox::warning(this, tr("警告！"),tr("没有此记录！！"));
             ui->lineEdit_grade->clear();
             ui->lineEdit_id->clear();
             ui->lineEdit_name->clear();
             ui->label_photo->clear();
             myCom->write(temp1_n);
         }
     }
}

int MainWindow::String2Hex(char *str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen=0;
    int len=strlen(str);
    //senddata.SetSize(len/2);
    for(int i=0;i<len;)
    {
        char lstr,hstr=str[i];
        if(hstr==' ')
        {
            i++;
            continue;
        }
        i++;
        if(i>=len)
            break;
        lstr=str[i];
        hexdata=ConvertHexChar(hstr);
        lowhexdata=ConvertHexChar(lstr);
        if((hexdata==16)||(lowhexdata==16))
            break;
        else
            hexdata=hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen]=(char)hexdata;
        hexdatalen++;
    }
    //senddata[hexdatalen]='\0';
    //senddata.SetSize(hexdatalen);
    return hexdatalen;
}

char MainWindow::ConvertHexChar(char ch)
{
    if((ch>='0')&&(ch<='9'))
        return ch-0x30;
    else if((ch>='A')&&(ch<='F'))
        return ch-'A'+10;
    else if((ch>='a')&&(ch<='f'))
        return ch-'a'+10;
    else return (-1);
}


void MainWindow::on_pushButton_chuqin_clicked()
{
    if((ui->comboBox_class->currentText()=="")
            ||(ui->comboBox_classname->currentText()=="")
            ||(ui->comboBox_classtime->currentText()=="")
            ||(ui->comboBox_teacher->currentText()==""))
    {
        QMessageBox::warning(this, tr("警告！"),tr("请先将课程信息填充完整!!"));
        return;
    }
    QStringList str1,str2;
    str1.append(ui->comboBox_classtime->currentText());
    str1.append(ui->comboBox_classname->currentText());
    str1.append(ui->comboBox_teacher->currentText());
    str1.append(ui->comboBox_class->currentText());
    QSqlQuery query;
    query.exec("select * from '"+ui->comboBox_class->currentText()+"'");
    query.last();
    int num=query.at();
    query.first();

    str1.append(QString::number((num+1),10));

    int nums=0;
    query.exec("select * from '"+ui->comboBox_class->currentText()+"' where flag='"+"0"+"'");
    while(query.next())
    {
        nums++;
        str2.append(tr("姓名："));
        str2.append(query.value(1).toString());
        str2.append(tr(" 学号："));
        str2.append(query.value(0).toString());
        str2.append("\n");

    }
    str1.append(QString::number((num+1-nums),10));


    Form *fi=new Form();
    SetFlags(0);
    ClearText();
    //fi->show();
    fi->showForm(str1,str2);
}

int MainWindow::SetFlags(int flags)
{
    int i=0;
    if(flags == 0)
    {
        QSqlQuery query;
        query.exec("update '"+ui->comboBox_class->currentText()+"' set flag='"+"0"+"' where flag='"+"1"+"'");
    }
    else if(flags == 1)
    {
        QSqlQuery query;
        query.exec("select * form '"+ui->comboBox_class->currentText()+"' where flag='"+"1"+"'");
        query.last();
        i = query.at() + 1;
    }
    return i;
}

void MainWindow::SetFlags(QString flags)
{
        QSqlQuery query;
        query.exec("update '"+ui->comboBox_class->currentText()+"' set flag='"+"1"+"' where id like '________"+flags+"'");
        //query.clear();
}
