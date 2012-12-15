#include <QtGui/QApplication>
#include "OgAdminWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ogAdminWindow w;
    w.show();

    return a.exec();
}
