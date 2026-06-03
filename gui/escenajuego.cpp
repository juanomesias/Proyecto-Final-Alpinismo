#include "escenajuego.h"

EscenaJuego::EscenaJuego(QWidget *parent)
    : QGraphicsView(parent),
    jugador(100, 500),
    gravedad(0.5f)
{
    escena = new QGraphicsScene(this);
    setScene(escena);
    etapa = 1;
    subEtapa = 1;


    QPixmap fondo(
        ":/resources/fondo_nivel1_1.png"
        );

    fondo = fondo.scaled(
        800,
        600,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    fondoVisual =
        escena->addPixmap(fondo);

    fondoVisual->setPos(0,0);

    fondoVisual->setZValue(-100);

    fondo = fondo.scaled(
        800,
        600,
        Qt::IgnoreAspectRatio,
        Qt::SmoothTransformation
        );

    escena->setSceneRect(
        0,
        0,
        800,
        600
        );

    spriteQuieto.load(
        ":/resources/sprites.png"
        );

    spriteQuieto =
        spriteQuieto.copy(
            0,
            0,
            64,
            64
            );

    jugadorVisual =
        escena->addPixmap(
            spriteQuieto.scaled(
                70,
                70
                )
            );
    plataformas.push_back(
        Plataforma(0,550,800,50)
        );

    for(const Plataforma& plataforma : plataformas)
    {
        QGraphicsPixmapItem *roca =
            escena->addPixmap(
                QPixmap(":/resources/sprites.png")
                    .copy(390,180,170,120)
                    .scaled(
                        plataforma.getAncho(),
                        plataforma.getAlto()
                        )
                );

        roca->setPos(
            plataforma.getX(),
            plataforma.getY()
            );
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

    if(jugador.getVelocidadY() != 0)
    {
        spriteSaltar =
            QPixmap(":/resources/sprites.png")
                .copy(120,0,64,64);

        jugadorVisual->setPixmap(
            spriteSaltar.scaled(70,70)
            );
    }
    else if(jugador.getVelocidadX() != 0)
    {
        spriteCorrer =
            QPixmap(":/resources/sprites.png")
                .copy(0,0,64,64);

        jugadorVisual->setPixmap(
            spriteCorrer.scaled(70,70)
            );
    }
    else
    {
        jugadorVisual->setPixmap(
            spriteQuieto.scaled(70,70)
            );
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

    if(
        jugador.getX() > 750 &&
        etapa == 1
        )
    {
        etapa = 2;

        jugador.setX(50);

        fondoVisual->setPixmap(
            QPixmap(
                ":/resources/fondo_nivel1_2.png"
                ).scaled(
                    800,
                    600
                    )
            );
    }

    if(
        jugador.getX() > 750 &&
        etapa == 2
        )
    {
        etapa = 3;

        jugador.setX(50);

        plataformas.push_back(
            Plataforma(150,470,180,25)
            );

        plataformas.push_back(
            Plataforma(400,420,180,25)
            );

        plataformas.push_back(
            Plataforma(100,340,180,25)
            );

        plataformas.push_back(
            Plataforma(450,280,180,25)
            );

        plataformas.push_back(
            Plataforma(200,200,180,25)
            );

        plataformas.push_back(
            Plataforma(500,130,180,25)
            );

        for(const Plataforma& plataforma : plataformas)
        {
            QGraphicsPixmapItem *roca =
                escena->addPixmap(
                    QPixmap(":/resources/sprites.png")
                        .copy(390,180,170,120)
                        .scaled(
                            plataforma.getAncho(),
                            plataforma.getAlto()
                            )
                    );

            roca->setPos(
                plataforma.getX(),
                plataforma.getY()
                );
        }

        if(etapa == 3)
        {
            if(
                jugador.getY() < 500 &&
                subEtapa == 1
                )
            {
                subEtapa = 2;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_4.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }

            if(
                jugador.getY() < 430 &&
                subEtapa == 2
                )
            {
                subEtapa = 3;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_5.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 360 &&
                subEtapa == 3
                )
            {
                subEtapa = 4;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_6.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 300 &&
                subEtapa == 4
                )
            {
                subEtapa = 5;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_7.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 240 &&
                subEtapa == 5
                )
            {
                subEtapa = 6;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_8.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 180 &&
                subEtapa == 6
                )
            {
                subEtapa = 7;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_9.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 120 &&
                subEtapa == 7
                )
            {
                subEtapa = 8;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_10.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 70 &&
                subEtapa == 8
                )
            {
                subEtapa = 9;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/fondo_nivel1_11.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }
            if(
                jugador.getY() < 20 &&
                subEtapa == 9
                )
            {
                subEtapa = 10;

                fondoVisual->setPixmap(
                    QPixmap(
                        ":/resources/Final1.png"
                        ).scaled(
                            800,
                            600
                            )
                    );
            }



        }

        fondoVisual->setPixmap(
            QPixmap(
                ":/resources/fondo_nivel1_3.png"
                ).scaled(
                    800,
                    600
                    )

            );

        obstaculos.push_back(
            Obstaculo(350,500,10,50,50)
            );

        obstaculos.push_back(
            Obstaculo(550,390,10,50,50)
            );

        obstaculos.push_back(
            Obstaculo(180,450,15,50,50)
            );

        obstaculos.push_back(
            Obstaculo(480,330,15,50,50)
            );

        obstaculos.push_back(
            Obstaculo(250,450,15,50,50)
            );

        obstaculos.push_back(
            Obstaculo(550,260,15,50,50)
            );

        obstaculos.push_back(
            Obstaculo(350,150,15,50,50)
            );

        obstaculos.push_back(
            Obstaculo(150,480,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(450,380,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(250,250,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(120,470,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(520,350,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(220,220,20,50,50)
            );

        obstaculos.push_back(
            Obstaculo(600,120,20,50,50)
            );

        obstaculos.clear();

        obstaculos.push_back(
            Obstaculo(250,400,25,50,50)
            );

        obstaculos.push_back(
            Obstaculo(500,250,25,50,50)
            );

        obstaculos.push_back(
            Obstaculo(300,350,30,50,50)
            );

        obstaculos.push_back(
            Obstaculo(550,180,30,50,50)
            );

        obstaculos.clear();

        fondoVisual->setPixmap(
            QPixmap(
                ":/resources/Final1.png"
                ).scaled(
                    800,
                    600
                    )
            );

        meta =
            escena->addRect(
                700,
                250,
                50,
                50,
                QPen(Qt::green),
                QBrush(Qt::green)
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
        }    }
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