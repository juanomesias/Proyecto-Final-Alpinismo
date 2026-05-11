#include <QApplication>
#include "gui/ventanajuego.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    VentanaJuego ventana;
    ventana.show();

    return a.exec();
}