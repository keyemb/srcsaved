#include <QtGui>
#include "keyboard.h"

KeyBoard::KeyBoard()
{
    setupUi(this);
    setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    display->setFocus();
    waitingForOperand = true;
    inputMode = iMode_Normal;
    caps_Lock = false;

    signalMapper=new QSignalMapper(this);
    setMapper();
    connectMapper();

    connect(signalMapper,SIGNAL(mapped(const QString&)),this,SLOT(setDispText(const QString&)));

    connect(toolButton_up,SIGNAL(clicked()),this,SLOT(onCursorUp()));
    connect(toolButton_down,SIGNAL(clicked()),this,SLOT(onCursorDown()));
    connect(toolButton_right,SIGNAL(clicked()),this,SLOT(onCursorRight()));
    connect(toolButton_left,SIGNAL(clicked()),this,SLOT(onCursorLeft()));

    connect(toolButton_Inv,SIGNAL(clicked()),this,SLOT(onInvMode()));
    connect(toolButton_enter,SIGNAL(clicked()),this,SLOT(onEnter()));
    connect(toolButton_backspace,SIGNAL(clicked()),this,SLOT(onBackspace()));
    connect(toolButton_capslock,SIGNAL(clicked()),this,SLOT(onCapslock()));
    connect(toolButton_esc,SIGNAL(clicked()),this,SLOT(onEsc()));
    connect(horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(onHorizontalSliderValueChanged(int)));
}

KeyBoard::~KeyBoard()
{

}


/*
* Name : void mouseMoveEvent(QMouseEvent *event),void mousePressEvent(QMouseEvent *event)
* Type : QEvent
* Func : realize drag the window of keyBoard by mouse press and move
* In   : QMouseEvent
* Out  : Null
*/
void KeyBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void KeyBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    if (event->button() == Qt::RightButton)
    {
        close();
    }

}

/*
* Name : void setMapper()
* Type : function
* Func : mapping the toolButton text to signalMapper
* In   : Null
* Out  : Null
*/
void KeyBoard::setMapper()
{
    //number
    signalMapper->setMapping(toolButton_0,toolButton_0->text());
    signalMapper->setMapping(toolButton_1,toolButton_1->text());
    signalMapper->setMapping(toolButton_2,toolButton_2->text());
    signalMapper->setMapping(toolButton_3,toolButton_3->text());
    signalMapper->setMapping(toolButton_4,toolButton_4->text());
    signalMapper->setMapping(toolButton_5,toolButton_5->text());
    signalMapper->setMapping(toolButton_6,toolButton_6->text());
    signalMapper->setMapping(toolButton_7,toolButton_7->text());
    signalMapper->setMapping(toolButton_8,toolButton_8->text());
    signalMapper->setMapping(toolButton_9,toolButton_9->text());

    //letter
    signalMapper->setMapping(toolButton_a,toolButton_a->text());
    signalMapper->setMapping(toolButton_b,toolButton_b->text());
    signalMapper->setMapping(toolButton_c,toolButton_c->text());
    signalMapper->setMapping(toolButton_d,toolButton_d->text());
    signalMapper->setMapping(toolButton_e,toolButton_e->text());
    signalMapper->setMapping(toolButton_f,toolButton_f->text());
    signalMapper->setMapping(toolButton_g,toolButton_g->text());
    signalMapper->setMapping(toolButton_h,toolButton_h->text());
    signalMapper->setMapping(toolButton_i,toolButton_i->text());
    signalMapper->setMapping(toolButton_j,toolButton_j->text());
    signalMapper->setMapping(toolButton_k,toolButton_k->text());
    signalMapper->setMapping(toolButton_l,toolButton_l->text());
    signalMapper->setMapping(toolButton_m,toolButton_m->text());
    signalMapper->setMapping(toolButton_n,toolButton_n->text());
    signalMapper->setMapping(toolButton_o,toolButton_o->text());
    signalMapper->setMapping(toolButton_p,toolButton_p->text());
    signalMapper->setMapping(toolButton_q,toolButton_q->text());
    signalMapper->setMapping(toolButton_r,toolButton_r->text());
    signalMapper->setMapping(toolButton_s,toolButton_s->text());
    signalMapper->setMapping(toolButton_t,toolButton_t->text());
    signalMapper->setMapping(toolButton_u,toolButton_u->text());
    signalMapper->setMapping(toolButton_v,toolButton_v->text());
    signalMapper->setMapping(toolButton_w,toolButton_w->text());
    signalMapper->setMapping(toolButton_x,toolButton_x->text());
    signalMapper->setMapping(toolButton_y,toolButton_y->text());
    signalMapper->setMapping(toolButton_z,toolButton_z->text());

    //other
    signalMapper->setMapping(toolButton_line1,toolButton_line1->text());
    signalMapper->setMapping(toolButton_line2,toolButton_line2->text());
    signalMapper->setMapping(toolButton_line3,toolButton_line3->text());
    signalMapper->setMapping(toolButton_wenhao,toolButton_wenhao->text());
    signalMapper->setMapping(toolButton_dot,toolButton_dot->text());
    signalMapper->setMapping(toolButton_space," ");
    signalMapper->setMapping(toolButton_tab,"\t");
}

/*
* Name : void connectMapper()
* Type : function
* Func : connect the toolButton signal(clicked()) to the signalMapper slots(map())
* In   : Null
* Out  : Null
*/
void KeyBoard::connectMapper()
{
    //number
    connect(toolButton_0,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_1,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_2,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_3,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_4,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_5,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_6,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_7,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_8,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_9,SIGNAL(clicked()),signalMapper,SLOT(map()));

    //letter
    connect(toolButton_a,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_b,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_c,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_d,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_e,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_f,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_g,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_h,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_i,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_j,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_k,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_l,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_m,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_n,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_o,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_p,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_q,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_r,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_s,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_t,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_u,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_v,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_w,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_x,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_y,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_z,SIGNAL(clicked()),signalMapper,SLOT(map()));

    //other
    connect(toolButton_line1,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_line2,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_line3,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_wenhao,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_dot,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_space,SIGNAL(clicked()),signalMapper,SLOT(map()));
    connect(toolButton_tab,SIGNAL(clicked()),signalMapper,SLOT(map()));
}

void KeyBoard::onCursorUp()
{
    QKeyEvent upPress(QEvent::KeyPress,Qt::Key_Up,Qt::NoModifier);
    QApplication::sendEvent(QApplication::focusWidget(),&upPress);
}

void KeyBoard::onCursorDown()
{
    QKeyEvent downPress(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier);
    QApplication::sendEvent(QApplication::focusWidget(),&downPress);
}

/*
* Name : void onCursorRight()
* Type : slot
* Func : cursor move right
* In   : Null
* Out  : Null
*/
void KeyBoard::onCursorRight()
{
    display->cursorForward(false,1);
}

/*
* Name : void onCursorLeft()
* Type : slot
* Func : cursor move left
* In   : Null
* Out  : Null
*/
void KeyBoard::onCursorLeft()
{
    display->cursorBackward(false,1);
}

/*
* Name : void onHorizontalSliderValueChanged(int value)
* Type : slot
* Func :
* In   : int
* Out  : Null
*/
void KeyBoard::onHorizontalSliderValueChanged(int value)
{
    horizontalSlider->setRange(0,255);
    qreal opacity=(qreal)((1.0/256.0)*value);
    setWindowOpacity(opacity);
    lcdNumber->display(value);
}

/*
* Name : void setDispText(const QString& text)
* Type : slot
* Func : insert and show text on display(QLineEdit)
* In   : QString
* Out  : Null
*/
void KeyBoard::setDispText(const QString& text)
{
    if(waitingForOperand)
    {
        display->clear();
        waitingForOperand = false;
    }
    display->insert(text);
}

/*
* Name : void onInvMode()
* Type : slot
* Func : invert input mode
* In   : Null
* Out  : Null
*/
void KeyBoard::onInvMode()
{
    if(inputMode == iMode_Normal)
    {
        inputMode = iMode_Passwd;
    }
    else if(inputMode == iMode_Passwd)
    {
        inputMode = iMode_Normal;
    }

    if(inputMode == iMode_Normal)
    {
        display->setEchoMode(QLineEdit::Normal);
    }
    else if(inputMode == iMode_Passwd)
    {
        display->setEchoMode(QLineEdit::Password);
    }
}

/*
* Name : void onCapslock()
* Type : slot
* Func : caps lock
* In   : Null
* Out  : Null
*/
void KeyBoard::onCapslock()
{
    caps_Lock = !caps_Lock;
    if(caps_Lock)
    {
        //letter
        toolButton_a->setText("A");
        toolButton_b->setText("B");
        toolButton_c->setText("C");
        toolButton_d->setText("D");
        toolButton_e->setText("E");
        toolButton_f->setText("F");
        toolButton_g->setText("G");
        toolButton_h->setText("H");
        toolButton_i->setText("I");
        toolButton_j->setText("J");
        toolButton_k->setText("K");
        toolButton_l->setText("L");
        toolButton_m->setText("M");
        toolButton_n->setText("N");
        toolButton_o->setText("O");
        toolButton_p->setText("P");
        toolButton_q->setText("Q");
        toolButton_r->setText("R");
        toolButton_s->setText("S");
        toolButton_t->setText("T");
        toolButton_u->setText("U");
        toolButton_v->setText("V");
        toolButton_w->setText("W");
        toolButton_x->setText("X");
        toolButton_y->setText("Y");
        toolButton_z->setText("Z");
        toolButton_line1->setText("<");
        toolButton_line2->setText(">");
        toolButton_line3->setText("-");
        toolButton_dot->setText("*");
        toolButton_wenhao->setText("#");
    }
    else
    {
        //letter
        toolButton_a->setText("a");
        toolButton_b->setText("b");
        toolButton_c->setText("c");
        toolButton_d->setText("d");
        toolButton_e->setText("e");
        toolButton_f->setText("f");
        toolButton_g->setText("g");
        toolButton_h->setText("h");
        toolButton_i->setText("i");
        toolButton_j->setText("j");
        toolButton_k->setText("k");
        toolButton_l->setText("l");
        toolButton_m->setText("m");
        toolButton_n->setText("n");
        toolButton_o->setText("o");
        toolButton_p->setText("p");
        toolButton_q->setText("q");
        toolButton_r->setText("r");
        toolButton_s->setText("s");
        toolButton_t->setText("t");
        toolButton_u->setText("u");
        toolButton_v->setText("v");
        toolButton_w->setText("w");
        toolButton_x->setText("x");
        toolButton_y->setText("y");
        toolButton_z->setText("z");
        toolButton_line1->setText("/");
        toolButton_line2->setText("\\");
        toolButton_line3->setText("_");
        toolButton_dot->setText(".");
        toolButton_wenhao->setText("?");
    }
    setMapper();
}

/*
* Name : void onEnter()
* Type : slot
* Func : enter and emit the signal to editor to set text
* In   : Null
* Out  : Null
*/
void KeyBoard::onEnter()
{
    waitingForOperand = true;
    text = display->text();
    emit setvalue(text);
    display->clear();
    this->close();
}

/*
* Name : void onEsc()
* Type : slot
* Func : Esc
* In   : Null
* Out  : Null
*/
void KeyBoard::onEsc()
{
    display->clear();
    text.clear();
    this->close();
}

/*
* Name : void onBackspace()
* Type : slot
* Func : backspace
* In   : Null
* Out  : Null
*/
void KeyBoard::onBackspace()
{
    display->backspace();
}
