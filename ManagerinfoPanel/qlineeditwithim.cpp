#include "qlineeditwithim.h"

QLineEditWithIM::QLineEditWithIM()
{
//#ifdef Q_WS_QWS
    im = new InputMethod;
    installEventFilter(im);
    connect(im->keyboard,SIGNAL(setvalue(QString)),this,SLOT(setText(QString)));
//#endif
}
