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

void momPrinter::on_pushButton_clicked()
{
    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;
}
