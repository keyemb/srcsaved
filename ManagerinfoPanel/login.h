#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "qlineeditwithim.h"

class QLabel;
class QLineEdit;
class QDialogButtonBox;

class QLogin : public QDialog
{
    Q_OBJECT

public:
    QLogin();
    ~QLogin();

public:

    QLabel *managerLabel;
    QLabel *passwdLabel;

    QLineEditWithIM *managerEdit;
    QLineEditWithIM *passwdEdit;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QDialogButtonBox *buttonBox;

signals:
    void Authorize();

private slots:
    void login();
    void cancel();

};

#endif // LOGIN_H
