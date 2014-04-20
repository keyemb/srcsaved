#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT
    
public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    void showForm(QStringList str1,QStringList str2);


private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form *ui;


};

#endif // FORM_H
