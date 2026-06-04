#include "escenajuego.h"

#include <algorithm>

static const int ANCHO_PANTALLA = 800;
static const int ALTO_PANTALLA  = 600;
static const int NUM_FONDOS     = 11;
static const int FONDOS_RUNNER  = 3;

EscenaJuego::EscenaJuego(QWidget *parent)
    : QGraphicsView(parent),
    jugador(100, 100),
    gravedad(0.5f)
{
    escena = new QGraphicsScene(this);
    setScene(escena);
    etapa    = 1;
    subEtapa = 1;

    const int mundoAncho = ANCHO_PANTALLA * FONDOS_RUNNER;
    const int mundoAlto  = ALTO_PANTALLA * (NUM_FONDOS - FONDOS_RUNNER + 1);
    const int pisoBaseY  = mundoAlto - ALTO_PANTALLA;

    escena->setSceneRect(0, 0, mundoAncho, mundoAlto);

    QStringList fondos = {
        ":/resources/fondo_nivel1_1.png",
        ":/resources/fondo_nivel1_2.png",
        ":/resources/fondo_nivel1_3.png",
        ":/resources/fondo_nivel1_4.png",
        ":/resources/fondo_nivel1_5.png",
        ":/resources/fondo_nivel1_6.png",
        ":/resources/fondo_nivel1_7.png",
        ":/resources/fondo_nivel1_8.png",
        ":/resources/fondo_nivel1_9.png",
        ":/resources/fondo_nivel1_10.png",
        ":/resources/Final1.png"
    };

    for(int i = 0; i < fondos.size(); i++)
    {
        QPixmap px(fondos[i]);
        if(px.isNull()) px = QPixmap(ANCHO_PANTALLA, ALTO_PANTALLA);

        px = px.scaled(ANCHO_PANTALLA, ALTO_PANTALLA,
                       Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        QGraphicsPixmapItem* item = escena->addPixmap(px);

        if(i < FONDOS_RUNNER)
        {
            item->setPos(ANCHO_PANTALLA * i, pisoBaseY);
        }
        else
        {
            const int alturaEscalada = i - FONDOS_RUNNER + 1;
            item->setPos(ANCHO_PANTALLA * (FONDOS_RUNNER - 1),
                         pisoBaseY - ALTO_PANTALLA * alturaEscalada);
        }

        item->setZValue(-100);
        fondosItems.push_back(item);
    }

    spriteQuieto.load(":/resources/sprites.png");
    spriteQuieto = spriteQuieto.copy(0, 0, 64, 64);
    jugadorVisual = escena->addPixmap(spriteQuieto.scaled(70, 70));
    jugadorVisual->setZValue(10);

    jugador.setX(100);
    jugador.setY(pisoBaseY + 450);

    construirMundo(mundoAlto);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(ANCHO_PANTALLA, ALTO_PANTALLA);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EscenaJuego::actualizarJuego);
    timer->start(16);

    setFocus();
}

EscenaJuego::~EscenaJuego() {}

void EscenaJuego::agregarPlataforma(float x, float yMundo, float ancho, float alto)
{
    plataformas.push_back(Plataforma(x, yMundo, ancho, alto));

    QPixmap spritePlataforma(":/resources/sprites.png");
    QGraphicsPixmapItem* roca = escena->addPixmap(
        spritePlataforma.copy(390, 180, 170, 120).scaled(ancho, alto));

    roca->setPos(x, yMundo);
    roca->setZValue(5);
    plataformasVisuales.push_back(roca);
}

void EscenaJuego::construirMundo(int mundoAlto)
{
    const int pisoBaseY = mundoAlto - ALTO_PANTALLA;
    const int columnaEscaladaX = ANCHO_PANTALLA * (FONDOS_RUNNER - 1);

    auto YBase = [&](float yLocal) -> float
    {
        return pisoBaseY + yLocal;
    };

    auto YEscalada = [&](int fondo, float yLocal) -> float
    {
        return pisoBaseY - ALTO_PANTALLA * (fondo - FONDOS_RUNNER) + yLocal;
    };

    auto XEscalada = [&](float xLocal) -> float
    {
        return columnaEscaladaX + xLocal;
    };

    // Fondos 1, 2 y 3: recorrido horizontal.
    agregarPlataforma(0, YBase(550), ANCHO_PANTALLA * FONDOS_RUNNER, 50);

    // Fondo 3: primeras plataformas para empezar a subir.
    agregarPlataforma(XEscalada(590), YBase(450), 130, 25);
    agregarPlataforma(XEscalada(410), YBase(350), 130, 25);
    agregarPlataforma(XEscalada(230), YBase(250), 130, 25);
    agregarPlataforma(XEscalada(80),  YBase(150), 130, 25);
    agregarPlataforma(XEscalada(360), YBase(50),  130, 25);

    // Desde aqui cada fondo queda encima del anterior.
    for(int fondo = 4; fondo <= 10; fondo++)
    {
        const bool derecha = (fondo % 2 == 0);

        agregarPlataforma(XEscalada(derecha ? 120 : 550), YEscalada(fondo, 550), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 340 : 330), YEscalada(fondo, 450), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 560 : 110), YEscalada(fondo, 350), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 360 : 350), YEscalada(fondo, 250), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 130 : 560), YEscalada(fondo, 150), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 430 : 250), YEscalada(fondo, 50),  140, 25);
    }

    // Fondo final.
    agregarPlataforma(XEscalada(170), YEscalada(11, 550), 170, 25);
    agregarPlataforma(XEscalada(420), YEscalada(11, 410), 170, 25);
    agregarPlataforma(XEscalada(300), YEscalada(11, 260), 200, 25);
    agregarPlataforma(XEscalada(470), YEscalada(11, 120), 170, 25);

    meta = escena->addRect(
        XEscalada(520),
        YEscalada(11, 85),
        70,
        70,
        QPen(Qt::yellow),
        QBrush(Qt::yellow)
    );

    meta->setZValue(8);
}

void EscenaJuego::limpiarPlataformas()
{
    for(QGraphicsPixmapItem* v : plataformasVisuales)
    {
        escena->removeItem(v);
        delete v;
    }

    plataformasVisuales.clear();
    plataformas.clear();
}

void EscenaJuego::limpiarObstaculos()
{
    for(QGraphicsRectItem* v : obstaculosVisuales)
    {
        escena->removeItem(v);
        delete v;
    }

    obstaculosVisuales.clear();
    obstaculos.clear();
}

void EscenaJuego::cargarNivel(int, float) {}
void EscenaJuego::cambiarNivel() {}
void EscenaJuego::cambiarFondoPorEtapa() {}
void EscenaJuego::cargarEscalada(int) {}

void EscenaJuego::actualizarJuego()
{
    gravedad.aplicar(&jugador);
    jugador.actualizar();
    jugador.setEnSuelo(false);

    for(const Plataforma& p : plataformas)
    {
        float jx = jugador.getX();
        float jy = jugador.getY();
        const float JW = 50;
        const float JH = 50;

        const bool colX = jx + JW > p.getX() && jx < p.getX() + p.getAncho();
        const bool colY = jy + JH >= p.getY() && jy + JH <= p.getY() + p.getAlto();

        if(colX && colY && jugador.getVelocidadY() >= 0)
        {
            jugador.setY(p.getY() - JH);
            jugador.setVelocidadY(0);
            jugador.setEnSuelo(true);
        }
    }

    for(const Obstaculo& obs : obstaculos)
    {
        const float JW = 50;
        const float JH = 50;

        const bool colX = jugador.getX() + JW > obs.getX() &&
                          jugador.getX()      < obs.getX() + obs.getAncho();
        const bool colY = jugador.getY() + JH > obs.getY() &&
                          jugador.getY()      < obs.getY() + obs.getAlto();

        if(colX && colY) jugador.recibirDanio(obs.getDanio());
    }

    const QRectF limiteMundo = escena->sceneRect();
    jugador.setX(std::clamp(jugador.getX(), 0.0f,
                            static_cast<float>(limiteMundo.width() - 50)));

    if(jugador.getY() > limiteMundo.bottom() - 50)
    {
        jugador.setY(limiteMundo.bottom() - 50);
        jugador.setVelocidadY(0);
        jugador.setEnSuelo(true);
    }

    if(jugador.getY() < limiteMundo.top())
    {
        jugador.setY(limiteMundo.top());
        jugador.setVelocidadY(0);
    }

    if(jugador.getVelocidadY() != 0)
    {
        spriteSaltar = QPixmap(":/resources/sprites.png").copy(120, 0, 64, 64);
        jugadorVisual->setPixmap(spriteSaltar.scaled(70, 70));
    }
    else if(jugador.getVelocidadX() != 0)
    {
        spriteCorrer = QPixmap(":/resources/sprites.png").copy(0, 0, 64, 64);
        jugadorVisual->setPixmap(spriteCorrer.scaled(70, 70));
    }
    else
    {
        jugadorVisual->setPixmap(spriteQuieto.scaled(70, 70));
    }

    jugadorVisual->setPos(jugador.getX(), jugador.getY());

    for(size_t i = 0; i < obstaculos.size(); i++)
    {
        obstaculosVisuales[i]->setPos(obstaculos[i].getX(), obstaculos[i].getY());
    }

    if(meta)
    {
        QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

        if(jugadorRect.intersects(meta->sceneBoundingRect()))
        {
            timer->stop();

            QGraphicsTextItem* texto = escena->addText("NIVEL COMPLETADO");
            texto->setDefaultTextColor(Qt::yellow);
            texto->setScale(3);
            texto->setPos(jugador.getX() - 120, jugador.getY() - 120);
            texto->setZValue(20);
        }
    }

    centerOn(jugador.getX() + 25, jugador.getY() + 25);
}

void EscenaJuego::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)     jugador.setVelocidadX(-5);
    if(event->key() == Qt::Key_D)     jugador.setVelocidadX(5);
    if(event->key() == Qt::Key_Space) jugador.saltar();
}

void EscenaJuego::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_D)
        jugador.setVelocidadX(0);
}
