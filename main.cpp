#include "Widget.h"
#include "SettingsDialog.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // QSettings会用到
    QApplication::setOrganizationName("PikachuHy");
    QApplication::setOrganizationDomain("pikachu.net.cn");
    QApplication::setApplicationName("MyNotes");

    auto settingsDialog = new SettingsDialog();
    auto ret = settingsDialog->exec();
    if (ret == QDialog::Accepted) {
        Widget w;
        w.show();
        return a.exec();
    }
    return 0;
}
