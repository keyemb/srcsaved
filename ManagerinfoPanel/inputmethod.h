#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "keyboard.h"

class InputMethod : public KeyBoard
{
    Q_OBJECT
public:
    InputMethod();
    ~InputMethod();

    bool eventFilter(QObject *obj, QEvent *event);

public:
    KeyBoard *keyboard;

public:
    void showKeyBoard();
};

#endif // INPUTMETHOD_H
