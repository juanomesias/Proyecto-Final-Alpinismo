#include "escenajuego.h"

EscenaJuego::EscenaJuego(QWidget *parent)
    : QGraphicsView(parent),
    jugador(100, 100),
    gravedad(0.5f) {

    escena = new QGraphicsScene(this);
    setScene(escena);

    escena->setSceneRect(0, 0, 800, 600);

    jugadorVisual = escena->addRect(
        0,
        0,
        50,
        50,
        QPen(Qt::black),
        QBrush(Qt::white));

    sueloY = 500;
    escena->addRect(
        0,
        sueloY + 50,
        800,
        50,
        QPen(Qt::black),
        QBrush(Qt::darkGreen)
        );

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &EscenaJuego::actualizarJuego);
    timer->start(16);

    setFocus();
}

EscenaJuego::~EscenaJuego()
{
}

void EscenaJuego::actualizarJuego()
{
    gravedad.aplicar(&jugador);
    jugador.actualizar();

    if(jugador.getY() >= sueloY)
    {
        jugador.setY(sueloY);

        jugador.setVelocidadY(0);

        jugador.setEnSuelo(true);
    }

    jugadorVisual->setPos(
        jugador.getX(),
        jugador.getY()
        );
}

void EscenaJuego::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A){
        jugador.setVelocidadX(-5);
    }

    if(event->key() == Qt::Key_D){
        jugador.setVelocidadX(5);
    }

    if(event->key() == Qt::Key_Space){
        jugador.saltar();
    }
}

void EscenaJuego::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A ||
        event->key() == Qt::Key_D)
    {
        jugador.setVelocidadX(0);
    }
}