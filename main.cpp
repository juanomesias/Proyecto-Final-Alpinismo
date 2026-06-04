#include <QApplication>
#include "gui/menuprincipal.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MenuPrincipal menu;
    menu.show();

    return a.exec();
}
