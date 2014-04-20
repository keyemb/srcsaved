#include "mainwindow.h"
#include "ui_mainwindow.h"

#define CAM_DAT     "../Client/Camdata.txt"

QByteArray imageBuff;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tcpSocket = NULL;
    ui->txtPort->setText("8080");
    QString ip = loadHostIP();
    ui->txtIP->setText(ip);
    imageBuff.clear();
    lastbyte = 0;
    enRecv = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::Warning(const QString msg,const QString title)
{
    return  QMessageBox::information(this,title,msg);
}

bool MainWindow::isCorrectIP(QString str)
{
    QString pattern = "^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5]).(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5]).(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5]).(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$";
    QRegExp regex(pattern);
    return regex.exactMatch(str);
}

QString MainWindow::loadHostIP()
{
    QString ip = "127.0.0.1";

    QFile f(CONFIG_FILE);
    f.open(QIODevice::ReadOnly);
    QByteArray qba = f.readAll();

    QString tmp(qba);
    f.close();
    if(isCorrectIP(tmp)) ip = tmp;
    return ip;
}

void MainWindow::on_btnConnect_clicked()
{
    tcpSocket = new QTcpSocket(this);
    tcpSocket->abort();

    QString ip = ui->txtIP->text();
    int port = ui->txtPort->text().toInt();
    tcpSocket->connectToHost(ip,port);

    connect(tcpSocket,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readMesg()));
}

void MainWindow::onConnected()
{
    ui->btnConnect->setEnabled(false);
    QString str("GET /?action=stream");
    QByteArray qba = str.toAscii();
    qba += 0x0A;
    qba += 0x0A;
    tcpSocket->write(qba);
}


void MainWindow::readMesg() //读取信息
{
    QByteArray qba= tcpSocket->readAll(); //读取
    for(int i = 0;i < qba.count();i++)
    {
        if(enRecv) imageBuff += qba[i];
        if(lastbyte == (char)0xFF)
        {
            if(qba[i] == (char)0xD8)
            {
                qDebug()<<"SOI"<<endl;
                enRecv = true;
            }
            else if(qba[i] == (char)0xD9)
            {
                qDebug()<<"EOI"<<endl;
                QPixmap map;
                if(map.loadFromData(imageBuff,"JEPG"))
                {
                    ui->labelVideo->setPixmap(map);
                }
                else qDebug()<<"Error Format!"<<endl;
                imageBuff.clear();
                imageBuff += 0xFF;
                imageBuff += 0xD8;
                enRecv = false;
            }
        }
        lastbyte = qba[i];
    }
}

