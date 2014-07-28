#ifndef MOMPRINTER_H
#define MOMPRINTER_H

#include <QMainWindow>
#include <QPrinter>
#include <QPrintDialog>

namespace Ui {
class momPrinter;
}

class momPrinter : public QMainWindow
{
    Q_OBJECT

public:
    explicit momPrinter(QWidget *parent = 0);
    ~momPrinter();

private slots:
    void on_pushButton_clicked();

private:
    Ui::momPrinter *ui;
};

#endif // MOMPRINTER_H
