#include "momprinter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    momPrinter widget;

    widget.show();
    return app.exec();
}
