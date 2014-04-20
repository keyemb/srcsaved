#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "posix_qextserialport.h"
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    Posix_QextSerialPort *myCom;	//串口
    QTimer *readTimer;	//定时器 【此次不同于windows下的开发，非常重要】

    int String2Hex(char *str, QByteArray &senddata);
    char ConvertHexChar(char);
    void ShowDate(QString ids);
    void ClearText(void);
    int SetFlags(int flags);
    void SetFlags(QString flags);
    void serialinit(void); //串口初始化
    void file_read_write(void); //读取本地文件中的信息
    void choose_sd_udisk(void);

private slots:
    void readMyCom(); //读取串口 槽函数
    void on_pushButton_chuqin_clicked();
};

#endif // MAINWINDOW_H
