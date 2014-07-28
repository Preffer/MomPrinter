#ifndef MOMPRINTER_H
#define MOMPRINTER_H

#include <QMainWindow>

namespace Ui {
class momPrinter;
}

class momPrinter : public QMainWindow
{
    Q_OBJECT

public:
    explicit momPrinter(QWidget *parent = 0);
    ~momPrinter();

private:
    Ui::momPrinter *ui;
};

#endif // MOMPRINTER_H
