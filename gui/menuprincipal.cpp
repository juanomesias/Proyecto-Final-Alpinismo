#include "menuprincipal.h"
#include "ventanajuego.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QIcon>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSize>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

static const QString RUTA_RECURSOS = "D:/ProyectFinal/Proyecto-Final-Alpinismo/codigo/resources/";
static QMediaPlayer* musicaNivel1 = nullptr;
static QAudioOutput* salidaMusicaNivel1 = nullptr;

static QString rutaRecurso(const QString& nombreArchivo)
{
    const QStringList carpetas = {
        QCoreApplication::applicationDirPath() + "/resources",
        QCoreApplication::applicationDirPath() + "/../resources",
        QCoreApplication::applicationDirPath() + "/../../resources",
        QCoreApplication::applicationDirPath() + "/../../../resources",
        QDir::currentPath() + "/resources",
        RUTA_RECURSOS
    };

    for(const QString& carpeta : carpetas)
    {
        const QString ruta = QDir(carpeta).filePath(nombreArchivo);
        if(QFileInfo::exists(ruta))
            return ruta;
    }

    return QDir(RUTA_RECURSOS).filePath(nombreArchivo);
}

void iniciarMusicaMenuNivel1()
{
    if(!musicaNivel1)
    {
        musicaNivel1 = new QMediaPlayer;
        salidaMusicaNivel1 = new QAudioOutput;
        musicaNivel1->setAudioOutput(salidaMusicaNivel1);
        salidaMusicaNivel1->setVolume(0.1625f);
        musicaNivel1->setSource(QUrl::fromLocalFile(rutaRecurso("sounds/nivel1.mp3")));
        musicaNivel1->setLoops(QMediaPlayer::Infinite);
    }

    if(musicaNivel1->playbackState() != QMediaPlayer::PlayingState)
        musicaNivel1->play();
}

void detenerMusicaMenuNivel1()
{
    if(musicaNivel1)
        musicaNivel1->stop();
}

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

    iniciarMusicaMenuNivel1();
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
        "border-image: url(" + rutaRecurso("fondo_menu.png") + ") 0 0 0 0 stretch stretch;"
        "}"
    );

    botonIniciar = new QPushButton(central);
    prepararBotonImagen(botonIniciar, rutaRecurso("iniciojuego.png"), 280, 115);

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
        "border-image: url(" + rutaRecurso("fondo.png") + ") 0 0 0 0 stretch stretch;"
        "}"
    );

    botonFacil = new QPushButton(central);
    botonDificil = new QPushButton(central);
    prepararBotonImagen(botonFacil, rutaRecurso("nfacil.png"), 220, 82);
    prepararBotonImagen(botonDificil, rutaRecurso("ndificil.png"), 220, 82);

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
