#include "escenajuego.h"

#include <algorithm>
#include <QAudioOutput>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QUrl>
#include <stdexcept>

void detenerMusicaMenuNivel1();

static const int ANCHO_PANTALLA = 800;
static const int ALTO_PANTALLA  = 600;
static const int NUM_FONDOS     = 11;
static const int FONDOS_RUNNER  = 3;
static const int FONDOS_ESCALADA = 8;
static const int CANTIDAD_PIEDRAS = 8;
static const QString RUTA_RECURSOS = "D:/ProyectFinal/Proyecto-Final-Alpinismo/codigo/resources/";

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

static QString rutaSonido(const QString& nombreArchivo)
{
    return rutaRecurso("sounds/" + nombreArchivo);
}

static void exigirRecurso(const QString& nombreArchivo)
{
    const QString ruta = rutaRecurso(nombreArchivo);
    if(!QFileInfo::exists(ruta))
        throw std::runtime_error(("No se encontro el recurso: " + nombreArchivo).toStdString());
}

static void validarRecursosCriticos()
{
    const QStringList recursos = {
        "sprites.png",
        "spritesreves.png",
        "sprites2.png",
        "wasted.png",
        "repetir.png",
        "salir.png",
        "sounds/nivel1.mp3",
        "sounds/nivel2.mp3",
        "sounds/golpe_danio.mp3",
        "sounds/muerte.mp3"
    };

    for(const QString& recurso : recursos)
        exigirRecurso(recurso);
}

static QPixmap quitarFondoAzul(const QString& ruta)
{
    QImage imagen(ruta);
    if(imagen.isNull()) return QPixmap();

    imagen = imagen.convertToFormat(QImage::Format_ARGB32);

    for(int y = 0; y < imagen.height(); y++)
    {
        for(int x = 0; x < imagen.width(); x++)
        {
            QColor color = imagen.pixelColor(x, y);
            if(color.blue() > 120 &&
                color.blue() > color.red() + 35 &&
                color.blue() > color.green() + 25)
            {
                color.setAlpha(0);
                imagen.setPixelColor(x, y, color);
            }
        }
    }

    return QPixmap::fromImage(imagen);
}

static QPixmap quitarFondoNegro(const QPixmap& pixmap)
{
    QImage imagen = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    for(int y = 0; y < imagen.height(); y++)
    {
        for(int x = 0; x < imagen.width(); x++)
        {
            QColor color = imagen.pixelColor(x, y);
            if(color.red() < 18 && color.green() < 18 && color.blue() < 18)
            {
                color.setAlpha(0);
                imagen.setPixelColor(x, y, color);
            }
        }
    }

    return QPixmap::fromImage(imagen);
}

EscenaJuego::EscenaJuego(QWidget *parent, int dificultad)
    : QGraphicsView(parent),
    dificultad(dificultad),
    danioRoca(dificultad == 2 ? 50 : 25),
    jugador(100, 100),
    gravedad(0.5f)
{
    try
    {
        validarRecursosCriticos();
    }
    catch(const std::exception& error)
    {
        qWarning() << error.what();
    }

    escena = new QGraphicsScene(this);
    setScene(escena);
    etapa    = 1;
    subEtapa = 1;
    nivelJuego = 1;

    const int mundoAncho = ANCHO_PANTALLA * FONDOS_RUNNER;
    const int mundoAlto  = ALTO_PANTALLA * (FONDOS_ESCALADA + 2);
    const int pisoBaseY  = mundoAlto - ALTO_PANTALLA;

    escena->setSceneRect(0, 0, mundoAncho, mundoAlto);

    QStringList fondos = {
        rutaRecurso("fondo_nivel1_1.png"),
        rutaRecurso("fondo_nivel1_2.png"),
        rutaRecurso("fondo_nivel1_3.png"),
        rutaRecurso("fondo_nivel1_4.png"),
        rutaRecurso("fondo_nivel1_5.png"),
        rutaRecurso("fondo_nivel1_6.png"),
        rutaRecurso("fondo_nivel1_7.png"),
        rutaRecurso("fondo_nivel1_8.png"),
        rutaRecurso("fondo_nivel1_9.png"),
        rutaRecurso("fondo_nivel1_10.png"),
        rutaRecurso("fondo_nivel1_11.png"),
        rutaRecurso("Final1.png")
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
        else if(i == fondos.size() - 1)
            item->setPos(ANCHO_PANTALLA * (FONDOS_RUNNER - 1), 0);
        else
        {
            const int alturaEscalada = i - FONDOS_RUNNER + 1;
            item->setPos(ANCHO_PANTALLA * (FONDOS_RUNNER - 1),
                         pisoBaseY - ALTO_PANTALLA * alturaEscalada);
        }

        item->setZValue(-100);
        fondosItems.push_back(item);
    }

    QPixmap hojaSprites(rutaRecurso("sprites.png"));
    QPixmap hojaSpritesReves(rutaRecurso("spritesreves.png"));
    QPixmap hojaSprites2(rutaRecurso("sprites2.png"));
    spriteNivel1Quieto = quitarFondoNegro(hojaSprites.copy(45, 45, 260, 280));
    spriteNivel1Saltar = quitarFondoNegro(hojaSprites.copy(330, 45, 260, 290));
    spriteNivel1Cayendo = quitarFondoNegro(hojaSprites.copy(55, 340, 300, 230));
    spriteNivel1QuietoReves = quitarFondoNegro(hojaSpritesReves.copy(110, 85, 320, 380));
    spriteNivel1SaltarReves = quitarFondoNegro(hojaSpritesReves.copy(560, 80, 330, 400));
    spriteNivel1CayendoReves = quitarFondoNegro(hojaSpritesReves.copy(110, 520, 420, 275));
    spriteNivel2Quieto = quitarFondoNegro(hojaSprites2.copy(85, 95, 330, 345));
    spriteNivel2Saltar = quitarFondoNegro(hojaSprites2.copy(470, 35, 315, 365));
    spriteNivel2Cayendo = quitarFondoNegro(hojaSprites2.copy(760, 265, 490, 205));
    usarSpritesNivel1();
    spriteMuerto = quitarFondoNegro(QPixmap(rutaRecurso("Muerto.png")));
    spriteProteccion = quitarFondoNegro(QPixmap(rutaRecurso("proteccion.png")));
    spriteGolpe = quitarFondoNegro(QPixmap(rutaRecurso("golpe.png")));
    spriteGolpeEscudo = quitarFondoNegro(QPixmap(rutaRecurso("golpescudo.png")));
    spritePortal = hojaSprites2.copy(1235, 430, 405, 385).scaled(
        150, 155, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spritePortalGuia = quitarFondoNegro(hojaSprites2.copy(450, 505, 270, 335)).scaled(
        92, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spriteAyuda = quitarFondoNegro(QPixmap(rutaRecurso("vida.png"))).scaled(
        82, 82, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spritePlataformaNieve = quitarFondoNegro(hojaSprites.copy(440, 375, 390, 180));
    spriteCuracion = quitarFondoNegro(hojaSprites.copy(905, 380, 145, 190)).scaled(
        52, 52, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    spritePiso.load(rutaRecurso("piso.png"));
    spritePiso2.load(rutaRecurso("piso2.png"));
    spriteWasted = QPixmap(rutaRecurso("wasted.png"));
    spriteBotonRepetir = QPixmap(rutaRecurso("repetir.png"));
    spriteBotonSalir = QPixmap(rutaRecurso("salir.png"));

    jugadorVisual = escena->addPixmap(spriteQuieto.scaled(80, 80, Qt::KeepAspectRatio,
                                                          Qt::SmoothTransformation));
    jugadorVisual->setZValue(10);

    jugador.setX(100);
    jugador.setY(pisoBaseY + 450);

    construirMundo(mundoAlto);
    crearCuraciones();

    vidaFull.load(rutaRecurso("fullvida.png"));
    vidaUnoMenos.load(rutaRecurso("unomenos.png"));
    vidaMitad.load(rutaRecurso("mitadvida.png"));
    vidaPoca.load(rutaRecurso("pocavida.png"));
    vidaSin.load(rutaRecurso("sinvida.png"));

    vidaVisual = escena->addPixmap(vidaFull.scaled(115, 45, Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
    vidaVisual->setZValue(1000);

    cargarSpritesReloj();
    if(!spritesReloj.empty())
    {
        relojVisual = escena->addPixmap(spritesReloj.front());
        relojVisual->setZValue(1000);
        relojVisual->setVisible(false);
    }

    crearPiedras();
    crearAyuda();
    configurarAudio();

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

    QGraphicsPixmapItem* roca = escena->addPixmap(
        spritePlataformaNieve.scaled(ancho, alto, Qt::IgnoreAspectRatio,
                                     Qt::SmoothTransformation));

    roca->setPos(x, yMundo);
    roca->setZValue(5);
    plataformasVisuales.push_back(roca);
}

void EscenaJuego::agregarPiso(float x, float yMundo, float ancho, float alto)
{
    plataformas.push_back(Plataforma(x, yMundo, ancho, alto));

    QGraphicsPixmapItem* piso = escena->addPixmap(
        spritePiso.scaled(ancho, alto, Qt::IgnoreAspectRatio,
                          Qt::SmoothTransformation));

    piso->setPos(x, yMundo);
    piso->setZValue(4);
    plataformasVisuales.push_back(piso);
}

void EscenaJuego::usarSpritesNivel1()
{
    spriteQuieto = spriteNivel1Quieto;
    spriteSaltar = spriteNivel1Saltar;
    spriteCayendo = spriteNivel1Cayendo;
    spriteQuietoReves = spriteNivel1QuietoReves;
    spriteSaltarReves = spriteNivel1SaltarReves;
    spriteCayendoReves = spriteNivel1CayendoReves;
}

void EscenaJuego::usarSpritesNivel2()
{
    spriteQuieto = spriteNivel2Quieto;
    spriteSaltar = spriteNivel2Saltar;
    spriteCayendo = spriteNivel2Cayendo;
    spriteQuietoReves = spriteNivel2Quieto;
    spriteSaltarReves = spriteNivel2Saltar;
    spriteCayendoReves = spriteNivel2Cayendo;
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
    agregarPiso(0, YBase(550), ANCHO_PANTALLA * FONDOS_RUNNER, 50);

    QPixmap arbol = quitarFondoNegro(QPixmap(rutaRecurso("arbol.png"))).scaled(
        190, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QGraphicsPixmapItem* arbolDecorativo = escena->addPixmap(arbol);
    arbolDecorativo->setPos(95, YBase(390));
    arbolDecorativo->setZValue(6);

    // Fondo 3: primeras plataformas para empezar a subir.
    agregarPlataforma(XEscalada(590), YBase(450), 130, 25);
    agregarPlataforma(XEscalada(410), YBase(350), 130, 25);
    agregarPlataforma(XEscalada(230), YBase(250), 130, 25);
    agregarPlataforma(XEscalada(80),  YBase(150), 130, 25);
    agregarPlataforma(XEscalada(360), YBase(50),  130, 25);

    // Desde aqui cada fondo queda encima del anterior.
    for(int fondo = 4; fondo <= 11; fondo++)
    {
        const bool derecha = (fondo % 2 == 0);

        agregarPlataforma(XEscalada(derecha ? 120 : 550), YEscalada(fondo, 550), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 340 : 330), YEscalada(fondo, 450), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 560 : 110), YEscalada(fondo, 350), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 360 : 350), YEscalada(fondo, 250), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 130 : 560), YEscalada(fondo, 150), 140, 25);
        agregarPlataforma(XEscalada(derecha ? 430 : 250), YEscalada(fondo, 50),  140, 25);
    }

    // Final1 queda arriba de la escalada: medio piso para salir de la imagen 11 y caminar a la meta.
    agregarPiso(columnaEscaladaX + 300, 560, 500, 50);

    meta = escena->addPixmap(spritePortal);
    meta->setPos(columnaEscaladaX + 620, 415);
    meta->setZValue(8);

    QGraphicsPixmapItem* guiaPortal = escena->addPixmap(spritePortalGuia);
    guiaPortal->setPos(columnaEscaladaX + 500, 448);
    guiaPortal->setZValue(9);
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

void EscenaJuego::configurarAudio()
{
    musica = new QMediaPlayer(this);
    salidaMusica = new QAudioOutput(this);
    musica->setAudioOutput(salidaMusica);
    salidaMusica->setVolume(0.25f);

    sonidoDanio = new QMediaPlayer(this);
    salidaDanio = new QAudioOutput(this);
    sonidoDanio->setAudioOutput(salidaDanio);
    salidaDanio->setVolume(0.35f);
    sonidoDanio->setSource(QUrl::fromLocalFile(rutaSonido("golpe_danio.mp3")));

    sonidoMuerte = new QMediaPlayer(this);
    salidaMuerte = new QAudioOutput(this);
    sonidoMuerte->setAudioOutput(salidaMuerte);
    salidaMuerte->setVolume(0.45f);
    sonidoMuerte->setSource(QUrl::fromLocalFile(rutaSonido("muerte.mp3")));
}
void EscenaJuego::reproducirMusicaNivel1()
{
    if(!musica) return;

    musica->setSource(QUrl::fromLocalFile(rutaSonido("nivel1.mp3")));
    musica->setLoops(QMediaPlayer::Infinite);
    musica->play();
}

void EscenaJuego::reproducirMusicaNivel2()
{
    if(!musica) return;

    musica->setSource(QUrl::fromLocalFile(rutaSonido("nivel2.mp3")));
    musica->setLoops(QMediaPlayer::Infinite);
    musica->play();
}

void EscenaJuego::reproducirSonidoDanio()
{
    if(!sonidoDanio) return;

    sonidoDanio->stop();
    sonidoDanio->setPosition(0);
    sonidoDanio->play();
}

void EscenaJuego::reproducirSonidoMuerte()
{
    if(musica) musica->stop();
    if(!sonidoMuerte) return;

    sonidoMuerte->stop();
    sonidoMuerte->setPosition(0);
    sonidoMuerte->play();
}

void EscenaJuego::construirNivel2()
{
    escena->clear();
    usarSpritesNivel2();
    fondosItems.clear();
    plataformas.clear();
    obstaculos.clear();
    obstaculosVisuales.clear();
    powerups.clear();
    powerupsVisuales.clear();
    plataformasVisuales.clear();
    piedrasVisuales.clear();
    velocidadesPiedras.clear();
    curacionesVisuales.clear();

    meta = nullptr;
    ayudaVisual = nullptr;
    relojVisual = nullptr;
    vidaVisual = nullptr;
    wastedVisual = nullptr;
    botonRepetirVisual = nullptr;
    botonSalirVisual = nullptr;

    escena->setSceneRect(0, 0, ANCHO_PANTALLA * 3, ALTO_PANTALLA);

    for(int i = 0; i < 3; i++)
    {
        QPixmap fondo(rutaRecurso("fondo_nivel2_1.png"));
        fondo = fondo.scaled(ANCHO_PANTALLA, ALTO_PANTALLA,
                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QGraphicsPixmapItem* item = escena->addPixmap(fondo);
        item->setPos(ANCHO_PANTALLA * i, 0);
        item->setZValue(-100);
        fondosItems.push_back(item);
    }

    plataformas.push_back(Plataforma(0, 520, ANCHO_PANTALLA * 3, 80));
    QGraphicsPixmapItem* piso = escena->addPixmap(
        spritePiso2.scaled(ANCHO_PANTALLA * 3, 120, Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation));
    piso->setPos(0, 500);
    piso->setZValue(4);
    plataformasVisuales.push_back(piso);

    jugador.setX(80);
    jugador.setY(450);
    jugador.setVelocidadX(0);
    jugador.setVelocidadY(0);
    jugador.setEnSuelo(true);

    jugadorVisual = escena->addPixmap(spriteQuieto.scaled(80, 80, Qt::KeepAspectRatio,
                                                          Qt::SmoothTransformation));
    jugadorVisual->setZValue(10);
    jugadorVisual->setPos(jugador.getX(), jugador.getY());

    vidaVisual = escena->addPixmap(vidaFull.scaled(115, 45, Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
    vidaVisual->setZValue(1000);
    actualizarVidaVisual();
}

void EscenaJuego::cambiarANivel2()
{
    nivelJuego = 2;
    protegiendo = false;
    tipoImpacto = 0;
    potenciadorActivo = false;
    construirNivel2();
    detenerMusicaMenuNivel1();
    reproducirMusicaNivel2();
}

void EscenaJuego::recibirDanio(int cantidad)
{
    if(juegoTerminado || cantidad <= 0) return;

    vidaJugador = std::max(0, vidaJugador - cantidad);
    jugador.setVida(vidaJugador);
    actualizarVidaVisual();
    reproducirSonidoDanio();

    if(vidaJugador <= 0)
    {
        timer->stop();
        juegoTerminado = true;
        reproducirSonidoMuerte();
        mostrarPantallaMuerte();
    }
}

void EscenaJuego::mostrarPantallaMuerte()
{
    QRectF vista = mapToScene(viewport()->rect()).boundingRect();
    const QPointF centro = vista.center();

    if(jugadorVisual && !spriteMuerto.isNull())
    {
        jugadorVisual->setPixmap(spriteMuerto.scaled(95, 75, Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
        jugadorVisual->setPos(jugador.getX(), jugador.getY());
    }

    if(!spriteWasted.isNull())
    {
        wastedVisual = escena->addPixmap(spriteWasted.scaled(
            310, 170, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        wastedVisual->setPos(centro.x() - wastedVisual->boundingRect().width() / 2.0,
                             centro.y() - 180);
        wastedVisual->setZValue(2000);
    }

    const QPixmap repetir = spriteBotonRepetir.scaled(
        180, 126, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const QPixmap salir = spriteBotonSalir.scaled(
        180, 126, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    const float espacio = 24.0f;
    const float totalAncho = repetir.width() + salir.width() + espacio;
    const float yBotones = centro.y() + 20;

    botonRepetirVisual = escena->addPixmap(repetir);
    botonRepetirVisual->setPos(centro.x() - totalAncho / 2.0, yBotones);
    botonRepetirVisual->setZValue(2001);

    botonSalirVisual = escena->addPixmap(salir);
    botonSalirVisual->setPos(centro.x() - totalAncho / 2.0 + repetir.width() + espacio,
                             yBotones);
    botonSalirVisual->setZValue(2001);
}

void EscenaJuego::curarJugador(int cantidad)
{
    if(juegoTerminado || cantidad <= 0) return;

    vidaJugador = std::min(100, vidaJugador + cantidad);
    jugador.setVida(vidaJugador);
    actualizarVidaVisual();
}

void EscenaJuego::reiniciarNivelActual()
{
    if(musica) musica->stop();

    juegoTerminado = false;
    vidaJugador = 100;
    jugador.setVida(vidaJugador);
    jugador.setVelocidadX(0);
    jugador.setVelocidadY(0);
    jugador.setEnSuelo(true);
    protegiendo = false;
    potenciadorActivo = false;
    tipoImpacto = 0;
    estabaCayendo = false;

    if(nivelJuego == 2)
    {
        construirNivel2();
        reproducirMusicaNivel2();
    }
    else
    {
        if(wastedVisual) escena->removeItem(wastedVisual);
        if(botonRepetirVisual) escena->removeItem(botonRepetirVisual);
        if(botonSalirVisual) escena->removeItem(botonSalirVisual);
        delete wastedVisual;
        delete botonRepetirVisual;
        delete botonSalirVisual;
        wastedVisual = nullptr;
        botonRepetirVisual = nullptr;
        botonSalirVisual = nullptr;

        usarSpritesNivel1();
        jugador.setX(100);
        jugador.setY(escena->sceneRect().bottom() - 150);
        if(jugadorVisual)
        {
            jugadorVisual->setPixmap(spriteQuieto.scaled(80, 80, Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation));
            jugadorVisual->setPos(jugador.getX(), jugador.getY());
        }
        actualizarVidaVisual();
        centerOn(jugador.getX() + 25, jugador.getY() + 25);
    }

    if(timer) timer->start(16);
}

void EscenaJuego::mostrarImpacto(bool conEscudo)
{
    tipoImpacto = conEscudo ? 2 : 1;
    tiempoImpacto.restart();
}

void EscenaJuego::recibirImpactoObjeto(int cantidad)
{
    if(protegiendo)
    {
        mostrarImpacto(true);
        return;
    }

    mostrarImpacto(false);
    recibirDanio(cantidad);
}

void EscenaJuego::cargarSpritesReloj()
{
    QPixmap hojaReloj(rutaRecurso("relojarena.png"));
    if(hojaReloj.isNull()) return;

    const int cantidadSprites = 5;
    const int anchoSprite = hojaReloj.width() / cantidadSprites;

    for(int i = 0; i < cantidadSprites; i++)
    {
        QPixmap frame = hojaReloj.copy(i * anchoSprite, 230, anchoSprite, 520);
        spritesReloj.push_back(quitarFondoNegro(frame).scaled(
            58, 78, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void EscenaJuego::crearCuraciones()
{
    if(spriteCuracion.isNull() || plataformas.empty()) return;

    const std::vector<int> indices = {
        std::min(14, static_cast<int>(plataformas.size()) - 1),
        std::min(44, static_cast<int>(plataformas.size()) - 1)
    };

    for(int indice : indices)
    {
        const Plataforma& plataforma = plataformas[indice];
        QGraphicsPixmapItem* curacion = escena->addPixmap(spriteCuracion);
        curacion->setPos(
            plataforma.getX() + plataforma.getAncho() / 2.0f - spriteCuracion.width() / 2.0f,
            plataforma.getY() - spriteCuracion.height() - 8.0f
        );
        curacion->setZValue(14);
        curacionesVisuales.push_back(curacion);
    }
}

void EscenaJuego::actualizarCuraciones()
{
    QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

    for(QGraphicsPixmapItem* curacion : curacionesVisuales)
    {
        if(!curacion || !curacion->isVisible()) continue;

        if(curacion->sceneBoundingRect().intersects(jugadorRect))
        {
            curarJugador(25);
            curacion->setVisible(false);
        }
    }
}

void EscenaJuego::activarPotenciador()
{
    jugador.activarVelocidad();
    if(jugador.getVelocidadX() < 0)
        jugador.setVelocidadX(-jugador.getVelocidadActual());
    else if(jugador.getVelocidadX() > 0)
        jugador.setVelocidadX(jugador.getVelocidadActual());

    potenciadorActivo = true;
    tiempoPotenciador.restart();

    if(relojVisual && !spritesReloj.empty())
    {
        relojVisual->setPixmap(spritesReloj.front());
        relojVisual->setVisible(true);
    }
}

void EscenaJuego::actualizarPotenciador()
{
    if(!potenciadorActivo) return;

    const qint64 tiempoTranscurrido = tiempoPotenciador.elapsed();
    const int indiceSprite = std::min(
        static_cast<int>(spritesReloj.size()) - 1,
        static_cast<int>(tiempoTranscurrido / 1000)
    );

    if(relojVisual && !spritesReloj.empty() && indiceSprite >= 0)
        relojVisual->setPixmap(spritesReloj[indiceSprite]);

    if(tiempoTranscurrido >= 5000)
    {
        const float velocidadAnterior = jugador.getVelocidadX();
        jugador.desactivarVelocidad();
        if(velocidadAnterior < 0)
            jugador.setVelocidadX(-jugador.getVelocidadActual());
        else if(velocidadAnterior > 0)
            jugador.setVelocidadX(jugador.getVelocidadActual());

        potenciadorActivo = false;

        if(relojVisual)
            relojVisual->setVisible(false);
    }
}

void EscenaJuego::actualizarVidaVisual()
{
    if(!vidaVisual) return;

    QPixmap estadoVida;

    if(vidaJugador >= 100)
        estadoVida = vidaFull;
    else if(vidaJugador >= 75)
        estadoVida = vidaUnoMenos;
    else if(vidaJugador >= 50)
        estadoVida = vidaMitad;
    else if(vidaJugador >= 25)
        estadoVida = vidaPoca;
    else
        estadoVida = vidaSin;

    vidaVisual->setPixmap(estadoVida.scaled(115, 45, Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));
}

void EscenaJuego::crearPiedras()
{
    QPixmap piedra1 = quitarFondoAzul(rutaRecurso("piedra1.png"));
    QPixmap piedra2 = quitarFondoAzul(rutaRecurso("piedra2.png"));

    if(piedra1.isNull()) piedra1.load(rutaRecurso("piedra1.png"));
    if(piedra2.isNull()) piedra2.load(rutaRecurso("piedra2.png"));

    spritesPiedras.push_back(piedra1.scaled(50, 50, Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));
    spritesPiedras.push_back(piedra2.scaled(55, 55, Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation));

    for(int i = 0; i < CANTIDAD_PIEDRAS; i++)
    {
        QGraphicsPixmapItem* piedra = escena->addPixmap(spritesPiedras[i % spritesPiedras.size()]);
        piedra->setZValue(12);
        piedrasVisuales.push_back(piedra);
        velocidadesPiedras.push_back(4.0f);
        reiniciarPiedra(i);
    }
}

void EscenaJuego::reiniciarPiedra(int indice)
{
    if(indice < 0 || indice >= static_cast<int>(piedrasVisuales.size())) return;

    QRectF zonaVisible = mapToScene(viewport()->rect()).boundingRect();
    if(zonaVisible.isEmpty())
        zonaVisible = QRectF(0, 0, ANCHO_PANTALLA, ALTO_PANTALLA);

    const int spriteIndice = QRandomGenerator::global()->bounded(
        static_cast<int>(spritesPiedras.size()));
    const int margenX = 60;
    const int anchoZona = std::max(1, static_cast<int>(zonaVisible.width()) - margenX * 2);
    const float x = zonaVisible.left() + margenX +
                    QRandomGenerator::global()->bounded(anchoZona);
    const float y = zonaVisible.top() -
                    QRandomGenerator::global()->bounded(120, 520);

    piedrasVisuales[indice]->setPixmap(spritesPiedras[spriteIndice]);
    piedrasVisuales[indice]->setPos(x, y);
    velocidadesPiedras[indice] = 3.0f + QRandomGenerator::global()->bounded(3);
}

void EscenaJuego::actualizarPiedras()
{
    const float columnaEscaladaX = ANCHO_PANTALLA * (FONDOS_RUNNER - 1);
    const bool zonaFinalNivel1 = nivelJuego == 1 &&
                                 jugador.getX() >= columnaEscaladaX &&
                                 jugador.getY() <= 850;

    if(zonaFinalNivel1)
    {
        for(QGraphicsPixmapItem* piedra : piedrasVisuales)
            piedra->setVisible(false);
        return;
    }

    QRectF zonaVisible = mapToScene(viewport()->rect()).boundingRect();
    QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

    for(int i = 0; i < static_cast<int>(piedrasVisuales.size()); i++)
    {
        QGraphicsPixmapItem* piedra = piedrasVisuales[i];
        piedra->setVisible(true);
        piedra->setY(piedra->y() + velocidadesPiedras[i]);

        if(piedra->sceneBoundingRect().intersects(jugadorRect))
        {
            recibirImpactoObjeto(danioRoca);
            reiniciarPiedra(i);
            continue;
        }

        if(piedra->y() > zonaVisible.bottom() + 120)
            reiniciarPiedra(i);
    }
}

void EscenaJuego::crearAyuda()
{
    ayudaVisual = escena->addPixmap(spriteAyuda);
    ayudaVisual->setZValue(13);
    reiniciarAyuda();
}

void EscenaJuego::reiniciarAyuda()
{
    if(!ayudaVisual) return;

    QRectF zonaVisible = mapToScene(viewport()->rect()).boundingRect();
    if(zonaVisible.isEmpty())
        zonaVisible = QRectF(0, 0, ANCHO_PANTALLA, ALTO_PANTALLA);

    const int margenX = 70;
    const int anchoZona = std::max(1, static_cast<int>(zonaVisible.width()) - margenX * 2);
    const float x = zonaVisible.left() + margenX +
                    QRandomGenerator::global()->bounded(anchoZona);
    const float y = zonaVisible.top() -
                    QRandomGenerator::global()->bounded(300, 900);

    ayudaVisual->setPos(x, y);
    velocidadAyuda = 2.5f + QRandomGenerator::global()->bounded(3);
}

void EscenaJuego::actualizarAyuda()
{
    if(!ayudaVisual) return;

    QRectF zonaVisible = mapToScene(viewport()->rect()).boundingRect();
    QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

    ayudaVisual->setY(ayudaVisual->y() + velocidadAyuda);

    if(ayudaVisual->sceneBoundingRect().intersects(jugadorRect))
    {
        activarPotenciador();
        reiniciarAyuda();
        return;
    }

    if(ayudaVisual->y() > zonaVisible.bottom() + 160)
        reiniciarAyuda();
}

void EscenaJuego::actualizarHud()
{
    if(!vidaVisual) return;

    QRectF zonaVisible = mapToScene(viewport()->rect()).boundingRect();
    vidaVisual->setPos(zonaVisible.left() + 20, zonaVisible.top() + 20);

    if(relojVisual)
        relojVisual->setPos(zonaVisible.left() + 145, zonaVisible.top() + 14);
}

void EscenaJuego::actualizarJuego()
{
    if(juegoTerminado) return;

    gravedad.aplicar(&jugador);
    jugador.actualizar();
    jugador.setEnSuelo(false);

    if(jugador.getVelocidadY() > 0 && !estabaCayendo)
    {
        estabaCayendo = true;
        inicioCaidaY = jugador.getY();
    }

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
            if(estabaCayendo)
            {
                const float distanciaCaida = p.getY() - inicioCaidaY;
                if(distanciaCaida >= 700)
                    recibirDanio(100);
                else if(distanciaCaida >= 400)
                    recibirDanio(50);
                else if(distanciaCaida >= 200)
                    recibirDanio(25);

                estabaCayendo = false;
            }

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

        if(colX && colY) recibirImpactoObjeto(obs.getDanio());
    }

    actualizarPiedras();
    actualizarAyuda();
    actualizarCuraciones();
    actualizarPotenciador();

    const QRectF limiteMundo = escena->sceneRect();
    jugador.setX(std::clamp(jugador.getX(), 0.0f,
                            static_cast<float>(limiteMundo.width() - 50)));

    const float columnaEscaladaX = ANCHO_PANTALLA * (FONDOS_RUNNER - 1);
    if(nivelJuego == 1 && jugador.getX() >= columnaEscaladaX)
    {
        jugador.setX(std::clamp(jugador.getX(), columnaEscaladaX,
                                columnaEscaladaX + ANCHO_PANTALLA - 50.0f));
    }

    if(jugador.getY() > limiteMundo.bottom() - 50)
    {
        recibirDanio(50);
        jugador.setY(limiteMundo.bottom() - 50);
        jugador.setVelocidadY(0);
        jugador.setEnSuelo(true);
    }

    if(jugador.getY() < limiteMundo.top())
    {
        jugador.setY(limiteMundo.top());
        jugador.setVelocidadY(0);
    }

    if(juegoTerminado)
    {
        actualizarHud();
        return;
    }

    bool spriteEspecialActivo = false;

    if(tipoImpacto != 0)
    {
        if(tiempoImpacto.elapsed() >= 350)
        {
            tipoImpacto = 0;
        }
        else if(tipoImpacto == 2 && !spriteGolpeEscudo.isNull())
        {
            jugadorVisual->setPixmap(spriteGolpeEscudo.scaled(100, 100, Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation));
            spriteEspecialActivo = true;
        }
        else if(tipoImpacto == 1 && !spriteGolpe.isNull())
        {
            jugadorVisual->setPixmap(spriteGolpe.scaled(95, 95, Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation));
            spriteEspecialActivo = true;
        }
    }

    if(!spriteEspecialActivo && protegiendo && !spriteProteccion.isNull())
    {
        jugadorVisual->setPixmap(spriteProteccion.scaled(100, 100, Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation));
        spriteEspecialActivo = true;
    }

    if(!spriteEspecialActivo && jugador.getVelocidadY() < 0)
    {
        const QPixmap& sprite = mirandoIzquierda ? spriteSaltarReves : spriteSaltar;
        jugadorVisual->setPixmap(sprite.scaled(80, 80, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
    }
    else if(!spriteEspecialActivo && jugador.getVelocidadY() > 0)
    {
        const QPixmap& sprite = mirandoIzquierda ? spriteCayendoReves : spriteCayendo;
        jugadorVisual->setPixmap(sprite.scaled(85, 75, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
    }
    else if(!spriteEspecialActivo)
    {
        const QPixmap& sprite = mirandoIzquierda ? spriteQuietoReves : spriteQuieto;
        jugadorVisual->setPixmap(sprite.scaled(80, 80, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
    }

    jugadorVisual->setPos(jugador.getX(), jugador.getY());

    for(size_t i = 0; i < obstaculos.size(); i++)
    {
        obstaculosVisuales[i]->setPos(obstaculos[i].getX(), obstaculos[i].getY());
    }

    if(nivelJuego == 1 && meta)
    {
        QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

        if(jugadorRect.intersects(meta->sceneBoundingRect()))
        {
            cambiarANivel2();
            centerOn(jugador.getX() + 25, jugador.getY() + 25);
            actualizarHud();
            return;
        }
    }

    float camaraX = jugador.getX() + 25;
    float camaraY = jugador.getY() + 25;

    if(nivelJuego == 1 && jugador.getX() >= columnaEscaladaX)
    {
        camaraX = columnaEscaladaX + ANCHO_PANTALLA / 2.0f;
    }

    if(nivelJuego == 1 && jugador.getX() >= columnaEscaladaX && jugador.getY() < ALTO_PANTALLA)
        camaraY = ALTO_PANTALLA / 2.0f;

    camaraX = std::clamp(camaraX,
                         ANCHO_PANTALLA / 2.0f,
                         static_cast<float>(limiteMundo.width() - ANCHO_PANTALLA / 2.0f));
    camaraY = std::clamp(camaraY,
                         ALTO_PANTALLA / 2.0f,
                         static_cast<float>(limiteMundo.height() - ALTO_PANTALLA / 2.0f));

    centerOn(camaraX, camaraY);
    actualizarHud();
}

void EscenaJuego::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)
    {
        mirandoIzquierda = true;
        jugador.setVelocidadX(-jugador.getVelocidadActual());
    }

    if(event->key() == Qt::Key_D)
    {
        mirandoIzquierda = false;
        jugador.setVelocidadX(jugador.getVelocidadActual());
    }

    if(event->key() == Qt::Key_S)
    {
        protegiendo = true;
    }

    if(event->key() == Qt::Key_Space) jugador.saltar();
}

void EscenaJuego::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A || event->key() == Qt::Key_D)
        jugador.setVelocidadX(0);

    if(event->key() == Qt::Key_S)
        protegiendo = false;
}

void EscenaJuego::mousePressEvent(QMouseEvent *event)
{
    const QPointF punto = mapToScene(event->pos());

    if(juegoTerminado && botonRepetirVisual &&
        botonRepetirVisual->sceneBoundingRect().contains(punto))
    {
        reiniciarNivelActual();
        return;
    }

    if(juegoTerminado && botonSalirVisual &&
        botonSalirVisual->sceneBoundingRect().contains(punto))
    {
        qApp->quit();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}
