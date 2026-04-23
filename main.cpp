#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Limnmedia");
    QCoreApplication::setOrganizationDomain("limnmedia.com");
    QCoreApplication::setApplicationName("Virtual-Produino");

    MainWindow w;
    w.show();


    return QCoreApplication::exec();
}
