#include <QtGui>
#include "login.h"

QLogin::QLogin()
{
    managerLabel = new QLabel(tr("&Manager:"));
    managerEdit = new QLineEditWithIM;
    managerLabel->setBuddy(managerEdit);

    passwdLabel = new QLabel(tr("&Passwd:"));
    passwdEdit = new QLineEditWithIM;
    passwdEdit->setEchoMode(QLineEdit::Password);
    passwdLabel->setBuddy(passwdEdit);

    okButton = new QPushButton(tr("&Login"));
    cancelButton = new QPushButton("&Cancel");

    okButton->setDefault(true);

    buttonBox = new QDialogButtonBox;
    buttonBox->addButton(okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::AcceptRole);

    connect(okButton, SIGNAL(clicked()), this, SLOT(login()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(managerLabel);
    topLayout->addWidget(managerEdit);

    QHBoxLayout *midLayout = new QHBoxLayout;
    midLayout->addWidget(passwdLabel);
    midLayout->addWidget(passwdEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(midLayout);
    mainLayout->addWidget(buttonBox);
    mainLayout->setMargin(20);
    setLayout(mainLayout);
    managerEdit->setFocus();  

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/new/main/picture/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);
    setWindowTitle("Login");
}

QLogin::~QLogin()
{
    //qDebug()<<"login close";
    delete managerLabel;
    delete managerEdit;
    delete passwdLabel;
    delete passwdEdit;
    delete okButton;
    delete cancelButton;
}

/*
* Name : void login()
* Type : slot
* Func : login when authorize
* In   : Null
* Out  : Null
*/
void QLogin::login()
{
    qDebug()<<managerEdit->text();
    qDebug()<<passwdEdit->text();
}


/*
* Name : void cancel()
* Type : slot
* Func : cancel login
* In   : Null
* Out  : Null
*/
void QLogin::cancel()
{
    managerEdit->clear();
    passwdEdit->clear();
    close();
}

