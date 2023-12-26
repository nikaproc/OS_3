#ifndef FIRST_H
#define FIRST_H

#include <QDialog>

namespace Ui {
class first;
}

class first : public QDialog
{
    Q_OBJECT

public:
    explicit first(QWidget *parent = nullptr);
    ~first();

private slots:
    void on_returnButt_clicked();

    void on_pushButton_clicked();

private:
    Ui::first *ui;
    QString info;
};

#endif // FIRST_H
