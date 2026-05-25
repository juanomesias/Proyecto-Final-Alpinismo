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

    plataformas.push_back(
        Plataforma(0, 550, 800, 50)
        );

    plataformas.push_back(
        Plataforma(300, 400, 200, 30)
        );

    for(const Plataforma& plataforma : plataformas)
    {
        escena->addRect(
            plataforma.getX(),
            plataforma.getY(),
            plataforma.getAncho(),
            plataforma.getAlto(),
            QPen(Qt::black),
            QBrush(Qt::darkGreen)
            );
    }

    obstaculos.push_back(
        Obstaculo(500, 500, 20, 50, 50)
        );

    for(const Obstaculo& obstaculo : obstaculos)
    {
        QGraphicsRectItem* visual =
            escena->addRect(
                0,
                0,
                obstaculo.getAncho(),
                obstaculo.getAlto(),
                QPen(Qt::black),
                QBrush(Qt::red)
                );

        obstaculosVisuales.push_back(visual);
    }

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
    jugador.setEnSuelo(false);

    for(const Plataforma& plataforma : plataformas)
    {
        float jugadorX = jugador.getX();
        float jugadorY = jugador.getY();
        float jugadorAncho = 50;
        float jugadorAlto = 50;
        bool colisionX =
            jugadorX + jugadorAncho > plataforma.getX() &&
            jugadorX < plataforma.getX() + plataforma.getAncho();
        bool colisionY =
            jugadorY + jugadorAlto >= plataforma.getY() &&
            jugadorY + jugadorAlto <= plataforma.getY() + plataforma.getAlto();

        if(colisionX &&
            colisionY &&
            jugador.getVelocidadY() >= 0)
        {
            jugador.setY(
                plataforma.getY() - jugadorAlto
                );
            jugador.setVelocidadY(0);
            jugador.setEnSuelo(true);
        }
    }

    for(const Obstaculo& obstaculo : obstaculos)
    {
        float jugadorAncho = 50;
        float jugadorAlto = 50;
        bool colisionX =
            jugador.getX() + jugadorAncho > obstaculo.getX() &&
            jugador.getX() < obstaculo.getX() + obstaculo.getAncho();
        bool colisionY =
            jugador.getY() + jugadorAlto > obstaculo.getY() &&
            jugador.getY() < obstaculo.getY() + obstaculo.getAlto();

        if(colisionX && colisionY)
        {
            jugador.recibirDanio(
                obstaculo.getDanio()
                );
        }
    }

    jugadorVisual->setPos(
        jugador.getX(),
        jugador.getY()
        );

    for(size_t i = 0; i < obstaculos.size(); i++)
    {
        obstaculosVisuales[i]->setPos(
            obstaculos[i].getX(),
            obstaculos[i].getY()
            );
    }
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