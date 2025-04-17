#include "TLSApplikation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TLSApplikation w;
    w.show();
    return a.exec();
}
