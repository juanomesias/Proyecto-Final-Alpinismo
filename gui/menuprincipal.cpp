#include "menuprincipal.h"
#include "ventanajuego.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

MenuPrincipal::MenuPrincipal(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget;
    setCentralWidget(central);

    central->setStyleSheet(
        "QWidget {"
        "background-image: url(:/resources/fondo_menu.png);"
        "background-position: center;"
        "}"
        );

    titulo = new QLabel("ALPINISMO");

    titulo->setStyleSheet(
        "font-size: 40px;"
        "font-weight: bold;"
        "color: white;"
        );

    titulo->setAlignment(Qt::AlignCenter);

    botonIniciar = new QPushButton("INICIAR JUEGO");

    botonFacil = new QPushButton("FACIL");

    botonDificil = new QPushButton("DIFICIL");

    botonIniciar->setStyleSheet(
        "font-size: 20px;"
        "padding: 10px;"
        );

    botonFacil->setStyleSheet(
        "font-size: 18px;"
        "padding: 8px;"
        );

    botonDificil->setStyleSheet(
        "font-size: 18px;"
        "padding: 8px;"
        );

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addStretch();
    layout->addWidget(titulo);
    layout->addSpacing(30);
    layout->addWidget(botonFacil);
    layout->addWidget(botonDificil);
    layout->addWidget(botonIniciar);
    layout->addStretch();

    central->setLayout(layout);

    dificultad = 1;

    connect(botonFacil, &QPushButton::clicked,
            [=](){
                dificultad = 1;
            });

    connect(botonDificil, &QPushButton::clicked,
            [=](){
                dificultad = 2;
            });

    connect(botonIniciar, &QPushButton::clicked,
            [=](){

                VentanaJuego *juego =
                    new VentanaJuego();

                juego->show();

                this->close();
            });

    resize(800, 600);
}