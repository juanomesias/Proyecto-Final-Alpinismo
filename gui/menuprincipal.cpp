#include "menuprincipal.h"
#include "ventanajuego.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QVBoxLayout>
#include <QWidget>

MenuPrincipal::MenuPrincipal(QWidget *parent)
    : QMainWindow(parent),
    central(new QWidget(this)),
    botonIniciar(nullptr),
    botonFacil(nullptr),
    botonDificil(nullptr),
    dificultad(1)
{
    setCentralWidget(central);
    setFixedSize(800, 600);
    setWindowTitle("Alpinismo");

    mostrarInicio();
}

void MenuPrincipal::prepararBotonImagen(QPushButton *boton, const QString& ruta, int ancho, int alto)
{
    boton->setIcon(QIcon(ruta));
    boton->setIconSize(QSize(ancho, alto));
    boton->setFixedSize(ancho, alto);
    boton->setCursor(Qt::PointingHandCursor);
    boton->setStyleSheet(
        "QPushButton {"
        "border: none;"
        "background: transparent;"
        "}"
        "QPushButton:hover {"
        "padding-top: 2px;"
        "}"
    );
}

void MenuPrincipal::mostrarInicio()
{
    delete botonFacil;
    delete botonDificil;
    botonFacil = nullptr;
    botonDificil = nullptr;
    delete central->layout();

    central->setStyleSheet(
        "QWidget {"
        "background-image: url(:/resources/fondo_menu.png);"
        "background-position: center;"
        "background-repeat: no-repeat;"
        "}"
    );

    botonIniciar = new QPushButton(central);
    prepararBotonImagen(botonIniciar, ":/resources/iniciojuego.png", 280, 115);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addStretch();
    layout->addWidget(botonIniciar, 0, Qt::AlignCenter);
    layout->addSpacing(55);
    layout->addStretch();
    central->setLayout(layout);

    connect(botonIniciar, &QPushButton::clicked,
            this, &MenuPrincipal::mostrarSeleccionDificultad);
}

void MenuPrincipal::mostrarSeleccionDificultad()
{
    delete botonIniciar;
    botonIniciar = nullptr;
    delete central->layout();

    central->setStyleSheet(
        "QWidget {"
        "background-image: url(:/resources/fondo.png);"
        "background-position: center;"
        "background-repeat: no-repeat;"
        "}"
    );

    botonFacil = new QPushButton(central);
    botonDificil = new QPushButton(central);
    prepararBotonImagen(botonFacil, ":/resources/nfacil.png", 220, 82);
    prepararBotonImagen(botonDificil, ":/resources/ndificil.png", 220, 82);

    QVBoxLayout *botones = new QVBoxLayout;
    botones->addStretch();
    botones->addWidget(botonFacil, 0, Qt::AlignCenter);
    botones->addSpacing(18);
    botones->addWidget(botonDificil, 0, Qt::AlignCenter);
    botones->addStretch();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(3);
    layout->addLayout(botones, 1);
    layout->addSpacing(65);
    central->setLayout(layout);

    connect(botonFacil, &QPushButton::clicked,
            [=](){ iniciarJuego(1); });

    connect(botonDificil, &QPushButton::clicked,
            [=](){ iniciarJuego(2); });
}

void MenuPrincipal::iniciarJuego(int dificultadSeleccionada)
{
    dificultad = dificultadSeleccionada;

    VentanaJuego *juego = new VentanaJuego(nullptr, dificultad);
    juego->show();

    close();
}
