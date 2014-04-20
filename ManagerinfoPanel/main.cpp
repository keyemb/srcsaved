#include <QtGui/QApplication>
#include "login.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLogin login;
    login.show();
    return app.exec();
}
