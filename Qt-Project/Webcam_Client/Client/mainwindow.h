#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFile>

#define CONFIG_FILE  "../Client/CONFIG/CONFIG.DAT"
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int Warning(const QString msg,const QString title = "Tips");
    bool isCorrectIP(QString str);
    QString loadHostIP();
private slots:
    void on_btnConnect_clicked();
    void onConnected();
    void readMesg(); //∂¡»°–≈œ¢

private:
    Ui::MainWindow *ui;
    QTcpSocket* tcpSocket;
    char lastbyte;
    bool enRecv;
};

#endif // MAINWINDOW_H
