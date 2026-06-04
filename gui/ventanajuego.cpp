#include "ventanajuego.h"
#include "escenajuego.h"

VentanaJuego::VentanaJuego(QWidget *parent, int dificultad)
    : QMainWindow(parent)
{
    escena = new EscenaJuego(this, dificultad);

    setCentralWidget(escena);

    resize(800, 600);

    setWindowTitle("Proyecto Samurai");
}

VentanaJuego::~VentanaJuego()
{
}
