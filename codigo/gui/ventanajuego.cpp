#include "ventanajuego.h"
#include "escenajuego.h"

VentanaJuego::VentanaJuego(QWidget *parent)
    : QMainWindow(parent)
{
    escena = new EscenaJuego(this);

    setCentralWidget(escena);

    resize(800, 600);

    setWindowTitle("Proyecto Samurai");
}

VentanaJuego::~VentanaJuego()
{
}