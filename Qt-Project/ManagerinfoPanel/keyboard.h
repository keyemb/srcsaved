#ifndef  KeyBoard_H
#define  KeyBoard_H

#include <QtGui>
#include <QSignalMapper>
#include "ui_keyboard.h"

enum {
    iMode_Normal = 0,
    iMode_Passwd = 1
};

namespace Ui {
    class KeyBoard;
}

class KeyBoard : public QDialog, Ui::KeyBoard
{
    Q_OBJECT

public:
     KeyBoard();
     ~KeyBoard();

     void mouseMoveEvent(QMouseEvent *);
     void mousePressEvent(QMouseEvent *);

public:
     QString text;

private:
     QSignalMapper *signalMapper;
     QPoint dragPosition;
     bool caps_Lock;
     int inputMode;
     bool waitingForOperand;

private:
     void setMapper();
     void connectMapper();

signals:
     void setvalue(const QString &);

private slots:
     void onCursorRight();
     void onCursorLeft();
     void onCursorUp();
     void onCursorDown();

     void onHorizontalSliderValueChanged(int value);
     void setDispText(const QString& text);
     void onInvMode();

     void onCapslock();
     void onEnter();
     void onBackspace();
     void onEsc();
};

#endif //  KeyBoard_H
