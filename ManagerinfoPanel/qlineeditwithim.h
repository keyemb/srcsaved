#ifndef QLINEEDITWITHIM_H
#define QLINEEDITWITHIM_H

#include <QLineEdit>
#include "inputmethod.h"

class QLineEditWithIM : public QLineEdit
{
public:
    QLineEditWithIM();

private:
    InputMethod *im;
};

#endif // QLINEEDITWITHIM_H
