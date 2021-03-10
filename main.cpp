#include "Widget.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setOrganizationName("PikachuHy");
    QApplication::setOrganizationDomain("pikachu.net.cn");
    QApplication::setApplicationName("MyNotes");
    Widget w;
    w.show();
    return a.exec();
}
