#include "momprinter.h"
#include "ui_momprinter.h"

momPrinter::momPrinter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::momPrinter)
{
    ui->setupUi(this);
}

momPrinter::~momPrinter()
{
    delete ui;
}
