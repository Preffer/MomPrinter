#include "momprinter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QSharedPointer<QApplication> app = (QSharedPointer<QApplication>) new QApplication(argc, argv);
    QSharedPointer<momPrinter> widget = (QSharedPointer<momPrinter>) new momPrinter();
    widget->show();

    return app->exec();
}
