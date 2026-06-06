#include "escenajuego.h"

#include <algorithm>
#include <QAudioOutput>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
#include <QRandomGenerator>
#include <QUrl>
#include <cmath>
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

void EscenaJuego::actualizarMovimientoHorizontal()
{
    float velocidadHorizontal = 0.0f;
    const bool proteccionBloqueaAcciones = nivelJuego == 2 && protegiendo;
    const float velocidadJugador = nivelJuego == 2
                                       ? (jugador.getVelocidadExtra() ? 3.7f : 3.05f)
                                       : jugador.getVelocidadActual();

    if(nivelJuego == 2 &&
        faseNivel2 == FaseNivel2::Runner &&
        !juegoTerminado)
    {
        velocidadHorizontal += nivelRunner.getVelocidadScroll();
    }

    if(!proteccionBloqueaAcciones && teclaIzquierdaPresionada)
        velocidadHorizontal -= velocidadJugador;

    if(!proteccionBloqueaAcciones && teclaDerechaPresionada)
        velocidadHorizontal += velocidadJugador;

    jugador.setVelocidadX(velocidadHorizontal);

    if(teclaIzquierdaPresionada && !teclaDerechaPresionada)
        mirandoIzquierda = true;
    else if(teclaDerechaPresionada ||
             (nivelJuego == 2 &&
              faseNivel2 == FaseNivel2::Runner &&
              velocidadHorizontal >= 0.0f))
    {
        mirandoIzquierda = false;
    }
}

void EscenaJuego::crearTextoNivel2()
{
    QFont titulo("Arial", 15, QFont::Bold);
    QFont subtitulo("Arial", 10, QFont::Bold);

    textoNivel = escena->addText("Nivel 2: Huida de la IA", titulo);
    textoNivel->setDefaultTextColor(Qt::white);
    textoNivel->setZValue(1200);

    textoPuntaje = escena->addText("IA activa | J atacar | Shift cubrirse", subtitulo);
    textoPuntaje->setDefaultTextColor(QColor(255, 232, 150));
    textoPuntaje->setZValue(1200);

    textoTiempo = escena->addText("La IA te esta observando", subtitulo);
    textoTiempo->setDefaultTextColor(QColor(196, 230, 255));
    textoTiempo->setZValue(1200);
}

void EscenaJuego::iniciarAtaqueNivel2()
{
    if(nivelJuego != 2 || ataqueNivel2Activo || juegoTerminado || protegiendo)
        return;

    ataqueNivel2Activo = true;
    golpeJefeAplicadoAtaqueNivel2 = false;
    tiempoAtaqueNivel2.restart();

    if(!ataqueNivel2Visual)
    {
        ataqueNivel2Visual = escena->addRect(0, 0, 70, 45,
                                             QPen(Qt::NoPen),
                                             QBrush(QColor(255, 215, 90, 130)));
        ataqueNivel2Visual->setZValue(22);
    }

    ataqueNivel2Visual->setVisible(true);
    resolverAtaqueNivel2();
}

QRectF EscenaJuego::obtenerAreaAtaqueNivel2() const
{
    const float anchoAtaque = 80.0f;
    const float altoAtaque = 46.0f;
    const float x = mirandoIzquierda ? jugador.getX() - anchoAtaque + 8.0f
                                     : jugador.getX() + 42.0f;
    const float y = jugador.getY() + 14.0f;
    return QRectF(x, y, anchoAtaque, altoAtaque);
}

void EscenaJuego::actualizarAtaqueNivel2()
{
    if(!ataqueNivel2Visual)
        return;

    if(!ataqueNivel2Activo)
    {
        ataqueNivel2Visual->setVisible(false);
        return;
    }

    ataqueNivel2Visual->setRect(obtenerAreaAtaqueNivel2());

    if(tiempoAtaqueNivel2.elapsed() >= 180)
    {
        ataqueNivel2Activo = false;
        ataqueNivel2Visual->setVisible(false);
    }
}

void EscenaJuego::resolverAtaqueNivel2()
{
    if(!ataqueNivel2Activo)
        return;

    const QRectF areaAtaque = obtenerAreaAtaqueNivel2();

    for(Enemigo& enemigo : enemigosNivel2)
    {
        if(!enemigo.estaVivo())
            continue;

        QRectF rectEnemigo(enemigo.getX(), enemigo.getY(),
                           enemigo.getTipo() == VOLADOR ? 56.0f : 48.0f,
                           enemigo.getTipo() == VOLADOR ? 56.0f : 60.0f);

        if(areaAtaque.intersects(rectEnemigo))
        {
            const bool estabaVivo = enemigo.estaVivo();
            enemigo.recibirDanio(200);
            if(estabaVivo && !enemigo.estaVivo())
                enemigosDerrotadosNivel2++;
        }

        for(Proyectil& proyectil : enemigo.getProyectiles())
        {
            if(proyectil.estaActivo() &&
                areaAtaque.intersects(QRectF(proyectil.getX(), proyectil.getY(), 18.0f, 18.0f)))
            {
                proyectil.desactivar();
            }
        }
    }

    if(faseNivel2 == FaseNivel2::DueloFinal && perseguidorNivel2.estaVivo())
    {
        const QRectF rectBoss(perseguidorNivel2.getX(), perseguidorNivel2.getY(), 68.0f, 68.0f);
        if(!golpeJefeAplicadoAtaqueNivel2 && areaAtaque.intersects(rectBoss))
        {
            const int vidaAntes = perseguidorNivel2.getVida();
            perseguidorNivel2.recibirDanio(12);
            golpeJefeAplicadoAtaqueNivel2 = true;

            if(perseguidorNivel2.getVida() < vidaAntes)
            {
                golpesJefeNivel2++;
                tiempoAtaqueJefeNivel2 = 0.0f;
                if(golpesJefeNivel2 >= siguienteRafagaJefeNivel2)
                {
                    jefeRafagaCentralNivel2 = true;
                    jefeReposicionandoNivel2 = false;
                    tiempoRafagaJefeNivel2 = 0.0f;
                    proyectilesRafagaJefeNivel2 = 0;
                    siguienteRafagaJefeNivel2 += 10;
                }
                else
                {
                    elegirNuevaPosicionJefeNivel2();
                }
            }
        }
    }

    for(OndaOscilatoriaNivel2& onda : ondasOscilatoriasNivel2)
    {
        if(onda.activa && areaAtaque.intersects(QRectF(onda.x, onda.y, onda.ancho, onda.alto)))
            onda.activa = false;
    }
}

void EscenaJuego::sincronizarProyectilesEnemigo(const Enemigo& enemigo,
                                                std::vector<QGraphicsEllipseItem*>& visuales,
                                                const QColor& color)
{
    const std::vector<Proyectil>& proyectiles = enemigo.getProyectiles();

    while(visuales.size() < proyectiles.size())
    {
        QGraphicsEllipseItem* visual = escena->addEllipse(0, 0, 18, 18,
                                                          QPen(Qt::NoPen),
                                                          QBrush(color));
        visual->setZValue(21);
        visuales.push_back(visual);
    }

    while(visuales.size() > proyectiles.size())
    {
        QGraphicsEllipseItem* visual = visuales.back();
        escena->removeItem(visual);
        delete visual;
        visuales.pop_back();
    }

    for(size_t i = 0; i < proyectiles.size(); ++i)
    {
        visuales[i]->setPos(proyectiles[i].getX(), proyectiles[i].getY());
        visuales[i]->setVisible(proyectiles[i].estaActivo());
    }
}

void EscenaJuego::actualizarOndasOscilatoriasNivel2()
{
    const QRectF jugadorRect(jugador.getX(), jugador.getY(), 50.0f, 50.0f);

    while(ondasOscilatoriasNivel2Visuales.size() < ondasOscilatoriasNivel2.size())
    {
        QGraphicsEllipseItem* visual = escena->addEllipse(0, 0, 30, 30,
                                                          QPen(Qt::NoPen),
                                                          QBrush(QColor(123, 240, 255, 170)));
        visual->setZValue(23);
        ondasOscilatoriasNivel2Visuales.push_back(visual);
    }

    while(ondasOscilatoriasNivel2Visuales.size() > ondasOscilatoriasNivel2.size())
    {
        QGraphicsEllipseItem* visual = ondasOscilatoriasNivel2Visuales.back();
        escena->removeItem(visual);
        delete visual;
        ondasOscilatoriasNivel2Visuales.pop_back();
    }

    for(size_t i = 0; i < ondasOscilatoriasNivel2.size(); ++i)
    {
        OndaOscilatoriaNivel2& onda = ondasOscilatoriasNivel2[i];
        onda.x += onda.velocidadX;
        onda.y = onda.movimiento.calcularY(onda.yBase);

        if(onda.activa &&
            jugadorRect.intersects(QRectF(onda.x, onda.y, onda.ancho, onda.alto)))
        {
            onda.activa = false;
            recibirImpactoObjeto(onda.danio);
        }

        const bool fueraDeEscena = onda.x < -200.0f ||
                                   onda.x > escena->sceneRect().width() + 200.0f ||
                                   onda.y < -200.0f ||
                                   onda.y > escena->sceneRect().height() + 200.0f;

        if(fueraDeEscena)
            onda.activa = false;

        ondasOscilatoriasNivel2Visuales[i]->setRect(0, 0, onda.ancho, onda.alto);
        ondasOscilatoriasNivel2Visuales[i]->setPos(onda.x, onda.y);
        ondasOscilatoriasNivel2Visuales[i]->setVisible(onda.activa);
        ondasOscilatoriasNivel2Visuales[i]->setBrush(
            faseNivel2 == FaseNivel2::DueloFinal
                ? QBrush(QColor(255, 140, 80, 180))
                : QBrush(QColor(123, 240, 255, 170)));
    }

    for(size_t i = 0; i < ondasOscilatoriasNivel2.size();)
    {
        if(!ondasOscilatoriasNivel2[i].activa)
        {
            QGraphicsEllipseItem* visual = ondasOscilatoriasNivel2Visuales[i];
            escena->removeItem(visual);
            delete visual;
            ondasOscilatoriasNivel2Visuales.erase(ondasOscilatoriasNivel2Visuales.begin() + i);
            ondasOscilatoriasNivel2.erase(ondasOscilatoriasNivel2.begin() + i);
            continue;
        }

        ++i;
    }
}

void EscenaJuego::lanzarOndaOscilatoriaNivel2(const Enemigo& origen,
                                              float amplitud,
                                              float velocidad,
                                              int danio)
{
    float direccion = origen.getX() <= jugador.getX() ? 1.0f : -1.0f;
    float xInicial = origen.getX() + (direccion > 0.0f ? 45.0f : -25.0f);
    float yInicial = origen.getY() + 18.0f;

    if(faseNivel2 == FaseNivel2::Runner)
    {
        const bool desdeDelante = QRandomGenerator::global()->bounded(2) == 0;
        direccion = desdeDelante ? -1.0f : 1.0f;
        xInicial = desdeDelante
                       ? std::min(inicioArenaFinalNivel2 - 120.0f, jugador.getX() + 560.0f)
                       : std::max(fronteraIANivel2 + 45.0f, jugador.getX() - 360.0f);
        yInicial = std::clamp(jugador.getY() + 18.0f,
                              215.0f,
                              455.0f);
    }

    ondasOscilatoriasNivel2.emplace_back(xInicial,
                                         yInicial,
                                         velocidad * direccion,
                                         amplitud,
                                         34.0f,
                                         34.0f,
                                         danio);
}

void EscenaJuego::invocarApoyoIANivel2()
{
    if(faseNivel2 != FaseNivel2::Runner || enemigosNivel2.size() >= 2)
        return;

    const bool jugadorArriba = jugador.getY() < 410.0f;
    const float xSpawn = std::min(inicioArenaFinalNivel2 - 280.0f,
                                  jugador.getX() + 700.0f + QRandomGenerator::global()->bounded(240));

    if(jugadorArriba)
    {
        Enemigo volador(xSpawn,
                        std::max(180.0f, jugador.getY() - 70.0f),
                        VOLADOR,
                        24,
                        dificultad == 2 ? 15 : 12);
        volador.setRangoDeteccion(300.0f);
        volador.setVelocidades(0.0f, 4.9f + (dificultad == 2 ? 0.4f : 0.0f));
        enemigosNivel2.push_back(std::move(volador));
    }
    else
    {
        Enemigo patrullero(xSpawn, 462.0f, PATRULLERO, 30, dificultad == 2 ? 13 : 10);
        patrullero.setLimites(xSpawn - 90.0f, xSpawn + 90.0f);
        patrullero.setVelocidades(1.8f + (dificultad == 2 ? 0.4f : 0.0f), 4.2f);
        enemigosNivel2.push_back(std::move(patrullero));
    }
}

void EscenaJuego::limpiarEntidadesSecundariasNivel2()
{
    for(QGraphicsRectItem* visual : enemigosNivel2Visuales)
    {
        escena->removeItem(visual);
        delete visual;
    }
    enemigosNivel2Visuales.clear();

    for(auto& proyectilesVisuales : proyectilesEnemigosNivel2Visuales)
    {
        for(QGraphicsEllipseItem* visual : proyectilesVisuales)
        {
            escena->removeItem(visual);
            delete visual;
        }
    }
    proyectilesEnemigosNivel2Visuales.clear();

    enemigosNivel2.clear();

    for(QGraphicsEllipseItem* visual : ondasOscilatoriasNivel2Visuales)
    {
        escena->removeItem(visual);
        delete visual;
    }
    ondasOscilatoriasNivel2Visuales.clear();
    ondasOscilatoriasNivel2.clear();

    for(QGraphicsEllipseItem* visual : proyectilesPerseguidorVisuales)
    {
        escena->removeItem(visual);
        delete visual;
    }
    proyectilesPerseguidorVisuales.clear();
    perseguidorNivel2.getProyectiles().clear();
}

void EscenaJuego::iniciarDueloFinalNivel2()
{
    faseNivel2 = FaseNivel2::DueloFinal;
    limpiarEntidadesSecundariasNivel2();
    impulsoPersecucionNivel2 = 0.0f;
    ataqueNivel2Activo = false;
    estabaCayendo = false;
    if(ataqueNivel2Visual)
        ataqueNivel2Visual->setVisible(false);

    fronteraIANivel2 = inicioArenaFinalNivel2 - 70.0f;
    jugador.setX(inicioArenaFinalNivel2 + 140.0f);
    jugador.setY(450.0f);
    jugador.setVelocidadX(0.0f);
    jugador.setVelocidadY(0.0f);
    jugador.setEnSuelo(true);

    perseguidorNivel2 = Enemigo(finArenaFinalNivel2 - 220.0f,
                                392.0f,
                                JEFE,
                                220,
                                dificultad == 2 ? 18 : 14);
    golpesJefeNivel2 = 0;
    siguienteRafagaJefeNivel2 = 10;
    proyectilesRafagaJefeNivel2 = 0;
    tiempoAtaqueJefeNivel2 = 0.0f;
    tiempoRafagaJefeNivel2 = 0.0f;
    jefeRafagaCentralNivel2 = false;
    elegirNuevaPosicionJefeNivel2();

    if(meta)
        meta->setVisible(false);

    if(muroArenaIzquierdoNivel2Visual)
    {
        muroArenaIzquierdoNivel2Visual->setRect(0, 0, 36, ALTO_PANTALLA);
        muroArenaIzquierdoNivel2Visual->setPos(inicioArenaFinalNivel2 - 40.0f, 0.0f);
        muroArenaIzquierdoNivel2Visual->setVisible(true);
    }

    if(muroArenaDerechoNivel2Visual)
    {
        muroArenaDerechoNivel2Visual->setRect(0, 0, 36, ALTO_PANTALLA);
        muroArenaDerechoNivel2Visual->setPos(finArenaFinalNivel2 + 10.0f, 0.0f);
        muroArenaDerechoNivel2Visual->setVisible(true);
    }

    if(textoNivel)
        textoNivel->setPlainText("Duelo Final: IA adaptativa");

    if(textoPuntaje)
        textoPuntaje->setPlainText("Golpea al jefe con J y esquiva sus patrones");

    if(textoTiempo)
        textoTiempo->setPlainText("La IA cambio de protocolo");
}

void EscenaJuego::elegirNuevaPosicionJefeNivel2()
{
    std::vector<QPointF> posiciones = {
        QPointF(inicioArenaFinalNivel2 + 170.0f, 458.0f),
        QPointF((inicioArenaFinalNivel2 + finArenaFinalNivel2) * 0.5f - 30.0f, 458.0f),
        QPointF(finArenaFinalNivel2 - 250.0f, 458.0f)
    };

    for(const Plataforma& plataforma : plataformas)
    {
        if(plataforma.getAlto() >= 70.0f ||
            plataforma.getX() < inicioArenaFinalNivel2 ||
            plataforma.getX() > finArenaFinalNivel2)
        {
            continue;
        }

        posiciones.push_back(QPointF(plataforma.getX() + plataforma.getAncho() * 0.5f - 31.0f,
                                     plataforma.getY() - 62.0f));
    }

    if(posiciones.empty())
        return;

    int indice = QRandomGenerator::global()->bounded(static_cast<int>(posiciones.size()));
    QPointF destino = posiciones[indice];

    for(int intentos = 0; intentos < 6; ++intentos)
    {
        const float distancia = std::abs(static_cast<float>(destino.x()) - perseguidorNivel2.getX());
        if(distancia > 180.0f)
            break;

        indice = QRandomGenerator::global()->bounded(static_cast<int>(posiciones.size()));
        destino = posiciones[indice];
    }

    objetivoJefeNivel2X = static_cast<float>(destino.x());
    objetivoJefeNivel2Y = static_cast<float>(destino.y());
    jefeReposicionandoNivel2 = true;
    jefeRafagaCentralNivel2 = false;
}

void EscenaJuego::lanzarRafagaCentralJefeNivel2()
{
    const float direcciones[][2] = {
        {1.0f, 0.0f},
        {-1.0f, 0.0f},
        {0.7f, -0.45f},
        {-0.7f, -0.45f},
        {0.75f, 0.38f},
        {-0.75f, 0.38f}
    };

    for(const auto& direccion : direcciones)
        perseguidorNivel2.lanzarProyectilDireccionado(direccion[0], direccion[1], 0.8f);

    lanzarOndaOscilatoriaNivel2(perseguidorNivel2,
                                62.0f,
                                6.2f,
                                dificultad == 2 ? 16 : 12);
}

void EscenaJuego::actualizarDueloFinalNivel2(float deltaTiempo, bool jugadorEnAireAhora)
{
    jugador.setX(std::clamp(jugador.getX(),
                            inicioArenaFinalNivel2 + 6.0f,
                            finArenaFinalNivel2 - 56.0f));

    perseguidorNivel2.percibirJugador(jugador.getX(),
                                      jugador.getY(),
                                      jugador.getVelocidadX(),
                                      jugador.getVelocidadY(),
                                      jugadorEnAireAhora,
                                      protegiendo,
                                      ataqueNivel2Activo,
                                      true);
    perseguidorNivel2.actualizarProyectiles();

    if(jefeRafagaCentralNivel2)
    {
        objetivoJefeNivel2X = (inicioArenaFinalNivel2 + finArenaFinalNivel2) * 0.5f - 34.0f;
        objetivoJefeNivel2Y = 458.0f;
        const float dx = objetivoJefeNivel2X - perseguidorNivel2.getX();
        const float dy = objetivoJefeNivel2Y - perseguidorNivel2.getY();
        const float distancia = std::sqrt(dx * dx + dy * dy);

        if(distancia > 4.0f)
        {
            const float velocidad = 4.1f;
            perseguidorNivel2.setX(perseguidorNivel2.getX() + (dx / distancia) * velocidad);
            perseguidorNivel2.setY(perseguidorNivel2.getY() + (dy / distancia) * velocidad);
            return;
        }

        tiempoRafagaJefeNivel2 += deltaTiempo;
        if(tiempoRafagaJefeNivel2 >= 0.48f && proyectilesRafagaJefeNivel2 < 5)
        {
            lanzarRafagaCentralJefeNivel2();
            tiempoRafagaJefeNivel2 = 0.0f;
            proyectilesRafagaJefeNivel2++;
        }

        if(proyectilesRafagaJefeNivel2 >= 5)
            elegirNuevaPosicionJefeNivel2();

        return;
    }

    if(jefeReposicionandoNivel2)
    {
        const float dx = objetivoJefeNivel2X - perseguidorNivel2.getX();
        const float dy = objetivoJefeNivel2Y - perseguidorNivel2.getY();
        const float distancia = std::sqrt(dx * dx + dy * dy);

        if(distancia > 5.0f)
        {
            const float velocidad = 3.25f;
            perseguidorNivel2.setX(perseguidorNivel2.getX() + (dx / distancia) * velocidad);
            perseguidorNivel2.setY(perseguidorNivel2.getY() + (dy / distancia) * velocidad);
            return;
        }

        perseguidorNivel2.setX(objetivoJefeNivel2X);
        perseguidorNivel2.setY(objetivoJefeNivel2Y);
        jefeReposicionandoNivel2 = false;
        tiempoAtaqueJefeNivel2 = 0.0f;
    }

    tiempoAtaqueJefeNivel2 += deltaTiempo;

    const float dxJugador = jugador.getX() - perseguidorNivel2.getX();
    const float dyJugador = jugador.getY() - perseguidorNivel2.getY();
    const float distanciaJugador = std::sqrt(dxJugador * dxJugador + dyJugador * dyJugador);

    if(distanciaJugador < 115.0f && tiempoAtaqueJefeNivel2 > 0.35f)
    {
        elegirNuevaPosicionJefeNivel2();
        return;
    }

    if(tiempoAtaqueJefeNivel2 >= 2.0f)
    {
        const bool usarOnda = jugadorEnAireAhora || QRandomGenerator::global()->bounded(3) == 0;
        if(usarOnda)
        {
            lanzarOndaOscilatoriaNivel2(perseguidorNivel2,
                                        76.0f,
                                        6.7f,
                                        dificultad == 2 ? 17 : 13);
        }
        else
        {
            const float objetivoX = jugador.getX() + jugador.getVelocidadX() * 0.35f;
            const float objetivoY = jugador.getY() + jugador.getVelocidadY() * 0.25f;
            perseguidorNivel2.lanzarProyectilDireccionado(objetivoX - perseguidorNivel2.getX(),
                                                          objetivoY - perseguidorNivel2.getY(),
                                                          0.5f);
        }

        tiempoAtaqueJefeNivel2 = 0.0f;
    }

    perseguidorNivel2.setX(std::clamp(perseguidorNivel2.getX(),
                                      inicioArenaFinalNivel2 + 24.0f,
                                      finArenaFinalNivel2 - 72.0f));
}

void EscenaJuego::sincronizarVisualesEnemigosNivel2()
{
    while(enemigosNivel2Visuales.size() < enemigosNivel2.size())
    {
        QGraphicsRectItem* visual = escena->addRect(0, 0, 48, 48,
                                                    QPen(Qt::NoPen),
                                                    QBrush(QColor(130, 28, 24)));
        visual->setZValue(18);
        enemigosNivel2Visuales.push_back(visual);
        proyectilesEnemigosNivel2Visuales.emplace_back();
    }

    while(enemigosNivel2Visuales.size() > enemigosNivel2.size())
    {
        for(QGraphicsEllipseItem* visualProyectil : proyectilesEnemigosNivel2Visuales.back())
        {
            escena->removeItem(visualProyectil);
            delete visualProyectil;
        }
        proyectilesEnemigosNivel2Visuales.pop_back();

        QGraphicsRectItem* visual = enemigosNivel2Visuales.back();
        escena->removeItem(visual);
        delete visual;
        enemigosNivel2Visuales.pop_back();
    }

    for(size_t i = 0; i < enemigosNivel2.size(); ++i)
    {
        const Enemigo& enemigo = enemigosNivel2[i];
        QGraphicsRectItem* visual = enemigosNivel2Visuales[i];

        QColor color(130, 28, 24);
        QSizeF dimensiones(48.0f, 58.0f);

        if(enemigo.getTipo() == VOLADOR)
        {
            color = QColor(35, 95, 150);
            dimensiones = QSizeF(56.0f, 56.0f);
        }
        else if(enemigo.getTipo() == JEFE)
        {
            color = QColor(170, 90, 18);
            dimensiones = QSizeF(62.0f, 62.0f);
        }

        visual->setBrush(QBrush(color));
        visual->setRect(0, 0, dimensiones.width(), dimensiones.height());
        visual->setPos(enemigo.getX(), enemigo.getY());
        visual->setVisible(enemigo.estaVivo());

        sincronizarProyectilesEnemigo(
            enemigo,
            proyectilesEnemigosNivel2Visuales[i],
            enemigo.getTipo() == JEFE ? QColor(255, 114, 79) : QColor(255, 234, 120));
    }

    if(perseguidorNivel2Visual)
    {
        perseguidorNivel2Visual->setPos(perseguidorNivel2.getX(), perseguidorNivel2.getY());
        perseguidorNivel2Visual->setVisible(perseguidorNivel2.estaVivo());
        perseguidorNivel2Visual->setBrush(
            faseNivel2 == FaseNivel2::DueloFinal
                ? QBrush(QColor(214, 98, 34))
                : QBrush(QColor(170, 32, 24)));
    }

    if(barreraIAVisual)
    {
        if(faseNivel2 == FaseNivel2::Runner)
        {
            barreraIAVisual->setVisible(true);
            barreraIAVisual->setRect(0, 0, std::max(0.0f, fronteraIANivel2), ALTO_PANTALLA);
        }
        else
        {
            barreraIAVisual->setVisible(false);
        }
    }

    sincronizarProyectilesEnemigo(perseguidorNivel2,
                                  proyectilesPerseguidorVisuales,
                                  QColor(255, 87, 51));
}

void EscenaJuego::actualizarNivel2()
{
    const float deltaTiempo = 1.0f / 60.0f;
    tiempoRunnerNivel2 += deltaTiempo;
    const bool jugadorEnAireAhora = !jugador.getEnSuelo() ||
                                    std::abs(jugador.getVelocidadY()) > 0.1f;
    const QRectF jugadorRect(jugador.getX(), jugador.getY(), 50.0f, 50.0f);
    actualizarMovimientoHorizontal();

    if(faseNivel2 == FaseNivel2::Runner)
    {
        nivelRunner.actualizarTiempo(deltaTiempo);
        nivelRunner.actualizar();

        if(impulsoPersecucionNivel2 > 0.0f)
            impulsoPersecucionNivel2 = std::max(0.0f, impulsoPersecucionNivel2 - deltaTiempo);

        fronteraIANivel2 += nivelRunner.getVelocidadPersecucion() +
                            (impulsoPersecucionNivel2 > 0.0f ? 1.35f : 0.0f);

        perseguidorNivel2.percibirJugador(jugador.getX(),
                                          jugador.getY(),
                                          jugador.getVelocidadX(),
                                          jugador.getVelocidadY(),
                                          jugadorEnAireAhora,
                                          protegiendo,
                                          ataqueNivel2Activo,
                                          false);
        perseguidorNivel2.actualizar();

        if(perseguidorNivel2.getAtaqueIA() == AtaqueIA::ATAQUE_OSCILATORIO)
        {
            lanzarOndaOscilatoriaNivel2(perseguidorNivel2,
                                        70.0f,
                                        6.6f,
                                        dificultad == 2 ? 15 : 11);
            perseguidorNivel2.consumirDecisionIA();
        }
        else if(perseguidorNivel2.getAtaqueIA() == AtaqueIA::ATAQUE_ACELERAR_CAZA)
        {
            impulsoPersecucionNivel2 = 2.6f;
            perseguidorNivel2.consumirDecisionIA();
        }
        else if(perseguidorNivel2.getAtaqueIA() == AtaqueIA::ATAQUE_INVOCAR_APOYO)
        {
            invocarApoyoIANivel2();
            perseguidorNivel2.consumirDecisionIA();
        }

        if(perseguidorNivel2.getX() < fronteraIANivel2 + 35.0f)
            perseguidorNivel2.setX(fronteraIANivel2 + 35.0f);

        const QRectF rectPerseguidor(perseguidorNivel2.getX(), perseguidorNivel2.getY(), 68.0f, 68.0f);
        if((jugador.getX() <= fronteraIANivel2 + 55.0f || rectPerseguidor.intersects(jugadorRect)) &&
            (!tiempoDanioNivel2.isValid() || tiempoDanioNivel2.elapsed() >= 350))
        {
            recibirImpactoObjeto(dificultad == 2 ? 18 : 12);
            tiempoDanioNivel2.restart();
            jugador.setX(std::max(jugador.getX(), fronteraIANivel2 + 62.0f));
        }
    }
    else if(faseNivel2 == FaseNivel2::DueloFinal)
    {
        actualizarDueloFinalNivel2(deltaTiempo, jugadorEnAireAhora);

        if(!perseguidorNivel2.estaVivo())
        {
            faseNivel2 = FaseNivel2::Completado;
            juegoTerminado = true;
            if(timer) timer->stop();

            if(textoNivel)
                textoNivel->setPlainText("Nivel 2 completado");

            if(textoPuntaje)
                textoPuntaje->setPlainText("La IA fue derrotada");

            if(textoTiempo)
                textoTiempo->setPlainText("Aprendiste su patron y sobreviviste");

            actualizarHud();
            return;
        }
    }

    for(Proyectil& proyectil : perseguidorNivel2.getProyectiles())
    {
        if(!proyectil.estaActivo())
            continue;

        if(jugadorRect.intersects(QRectF(proyectil.getX(), proyectil.getY(), 18.0f, 18.0f)))
        {
            proyectil.desactivar();
            recibirImpactoObjeto(proyectil.getDanio());
        }
    }

    for(size_t i = 0; i < enemigosNivel2.size();)
    {
        Enemigo& enemigo = enemigosNivel2[i];
        enemigo.percibirJugador(jugador.getX(),
                                jugador.getY(),
                                jugador.getVelocidadX(),
                                jugador.getVelocidadY(),
                                jugadorEnAireAhora,
                                protegiendo,
                                ataqueNivel2Activo,
                                false);
        enemigo.actualizar();

        const QRectF rectEnemigo(enemigo.getX(),
                                 enemigo.getY(),
                                 enemigo.getTipo() == VOLADOR ? 56.0f : 48.0f,
                                 enemigo.getTipo() == VOLADOR ? 56.0f : 58.0f);

        if(enemigo.estaVivo() &&
            rectEnemigo.intersects(jugadorRect) &&
            (!tiempoDanioNivel2.isValid() || tiempoDanioNivel2.elapsed() >= 350))
        {
            recibirImpactoObjeto(enemigo.getDanio());
            tiempoDanioNivel2.restart();
        }

        for(Proyectil& proyectil : enemigo.getProyectiles())
        {
            if(!proyectil.estaActivo())
                continue;

            if(jugadorRect.intersects(QRectF(proyectil.getX(), proyectil.getY(), 18.0f, 18.0f)))
            {
                proyectil.desactivar();
                recibirImpactoObjeto(proyectil.getDanio());
            }
        }

        const bool enemigoAtras = enemigo.getX() < fronteraIANivel2 - 220.0f;
        const bool enemigoLejano = enemigo.getX() > inicioArenaFinalNivel2 + 200.0f;
        const bool eliminar = !enemigo.estaVivo() ||
                              (faseNivel2 == FaseNivel2::Runner && (enemigoAtras || enemigoLejano));

        if(eliminar)
        {
            for(QGraphicsEllipseItem* visualProyectil : proyectilesEnemigosNivel2Visuales[i])
            {
                escena->removeItem(visualProyectil);
                delete visualProyectil;
            }
            proyectilesEnemigosNivel2Visuales.erase(proyectilesEnemigosNivel2Visuales.begin() + i);

            QGraphicsRectItem* visual = enemigosNivel2Visuales[i];
            escena->removeItem(visual);
            delete visual;
            enemigosNivel2Visuales.erase(enemigosNivel2Visuales.begin() + i);

            enemigosNivel2.erase(enemigosNivel2.begin() + i);
            continue;
        }

        ++i;
    }

    resolverAtaqueNivel2();
    actualizarAtaqueNivel2();
    actualizarOndasOscilatoriasNivel2();
    sincronizarVisualesEnemigosNivel2();

    if(textoPuntaje && faseNivel2 == FaseNivel2::Runner)
    {
        textoPuntaje->setPlainText(
            QString("IA manda la caza | bichos fuera: %1").arg(enemigosDerrotadosNivel2));
    }
    else if(textoPuntaje && faseNivel2 == FaseNivel2::DueloFinal)
    {
        textoPuntaje->setPlainText(
            QString("Jefe: %1 vida | J para golpear").arg(perseguidorNivel2.getVida()));
    }

    if(textoTiempo)
    {
        QString estadoIA;

        if(faseNivel2 == FaseNivel2::Runner)
        {
            switch(perseguidorNivel2.getEstado())
            {
            case OSCILANDO:
                estadoIA = "La IA detecto tu altura y lanzo una onda";
                break;
            case PRESIONANDO:
                estadoIA = "La IA acelera la persecucion";
                break;
            case INVOCANDO_APOYO:
                estadoIA = "La IA invoco apoyo";
                break;
            case PERSIGUIENDO:
                estadoIA = "La IA te esta estudiando";
                break;
            default:
                estadoIA = nivelRunner.getTormentaActiva()
                               ? "La IA entro en fase agresiva"
                               : "La IA analiza tu patron";
                break;
            }

            textoTiempo->setPlainText(
                QString("%1 | %2 s").arg(estadoIA).arg(tiempoRunnerNivel2, 0, 'f', 1));
        }
        else if(faseNivel2 == FaseNivel2::DueloFinal)
        {
            if(jefeRafagaCentralNivel2)
            {
                estadoIA = "Patron central: busca huecos entre disparos";
            }
            else if(jefeReposicionandoNivel2)
            {
                estadoIA = "La IA busca otra posicion";
            }
            else
            {
                estadoIA = "La IA mantiene distancia y dispara";
            }

            textoTiempo->setPlainText(estadoIA);
        }
    }
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
    nivelRunner = NivelRunner();
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
    enemigosNivel2.clear();
    enemigosNivel2Visuales.clear();
    proyectilesEnemigosNivel2Visuales.clear();
    proyectilesPerseguidorVisuales.clear();
    ondasOscilatoriasNivel2.clear();
    ondasOscilatoriasNivel2Visuales.clear();

    meta = nullptr;
    ayudaVisual = nullptr;
    relojVisual = nullptr;
    vidaVisual = nullptr;
    wastedVisual = nullptr;
    botonRepetirVisual = nullptr;
    botonSalirVisual = nullptr;
    perseguidorNivel2Visual = nullptr;
    barreraIAVisual = nullptr;
    ataqueNivel2Visual = nullptr;
    muroArenaIzquierdoNivel2Visual = nullptr;
    muroArenaDerechoNivel2Visual = nullptr;
    textoNivel = nullptr;
    textoPuntaje = nullptr;
    textoTiempo = nullptr;
    faseNivel2 = FaseNivel2::Runner;
    inicioArenaFinalNivel2 = nivelRunner.getInicioDueloFinal();
    finArenaFinalNivel2 = nivelRunner.getFinDueloFinal();

    escena->setSceneRect(0, 0, finArenaFinalNivel2 + 420.0f, ALTO_PANTALLA);

    const int cantidadFondos = static_cast<int>(std::ceil(escena->sceneRect().width() / ANCHO_PANTALLA));
    for(int i = 0; i < cantidadFondos; i++)
    {
        QPixmap fondo(rutaRecurso(i % 2 == 0 ? "fondo_nivel2_1.png" : "fondo_nivel2_2.png"));
        if(fondo.isNull())
            fondo.load(rutaRecurso("fondo_nivel2_3.png"));

        fondo = fondo.scaled(ANCHO_PANTALLA, ALTO_PANTALLA,
                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QGraphicsPixmapItem* item = escena->addPixmap(fondo);
        item->setPos(ANCHO_PANTALLA * i, 0);
        item->setZValue(-100);
        fondosItems.push_back(item);
    }

    for(const Plataforma& plataformaNivel2 : nivelRunner.getPlataformas())
    {
        plataformas.push_back(plataformaNivel2);

        const bool esPiso = plataformaNivel2.getAlto() >= 70.0f;
        QPixmap spriteBase = esPiso ? spritePiso2 : spritePlataformaNieve;
        if(spriteBase.isNull())
            spriteBase = esPiso ? spritePiso : spritePlataformaNieve;

        QGraphicsPixmapItem* visual = escena->addPixmap(
            spriteBase.scaled(plataformaNivel2.getAncho(),
                              esPiso ? 120.0f : plataformaNivel2.getAlto(),
                              Qt::IgnoreAspectRatio,
                              Qt::SmoothTransformation));

        visual->setPos(plataformaNivel2.getX(), esPiso ? 500.0f : plataformaNivel2.getY());
        visual->setZValue(esPiso ? 4 : 5);
        plataformasVisuales.push_back(visual);
    }

    for(const Obstaculo& obstaculoNivel2 : nivelRunner.getObstaculos())
    {
        obstaculos.push_back(obstaculoNivel2);
        QGraphicsRectItem* visual = escena->addRect(0, 0,
                                                    obstaculoNivel2.getAncho(),
                                                    obstaculoNivel2.getAlto(),
                                                    QPen(Qt::NoPen),
                                                    QBrush(QColor(80, 48, 38)));
        visual->setPos(obstaculoNivel2.getX(), obstaculoNivel2.getY());
        visual->setZValue(11);
        obstaculosVisuales.push_back(visual);
    }

    meta = escena->addPixmap(spritePortal);
    meta->setPos(inicioArenaFinalNivel2 - 60.0f, 358.0f);
    meta->setZValue(14);

    barreraIAVisual = escena->addRect(0, 0, 1, ALTO_PANTALLA,
                                      QPen(Qt::NoPen),
                                      QBrush(QColor(159, 33, 24, 95)));
    barreraIAVisual->setZValue(-10);

    muroArenaIzquierdoNivel2Visual = escena->addRect(0, 0, 36, ALTO_PANTALLA,
                                                     QPen(Qt::NoPen),
                                                     QBrush(QColor(60, 18, 14)));
    muroArenaIzquierdoNivel2Visual->setZValue(17);
    muroArenaIzquierdoNivel2Visual->setVisible(false);

    muroArenaDerechoNivel2Visual = escena->addRect(0, 0, 36, ALTO_PANTALLA,
                                                   QPen(Qt::NoPen),
                                                   QBrush(QColor(60, 18, 14)));
    muroArenaDerechoNivel2Visual->setZValue(17);
    muroArenaDerechoNivel2Visual->setVisible(false);

    perseguidorNivel2 = Enemigo(20.0f, 405.0f, JEFE, 170, dificultad == 2 ? 16 : 12);
    perseguidorNivel2Visual = escena->addRect(0, 0, 62, 62,
                                              QPen(Qt::NoPen),
                                              QBrush(QColor(170, 32, 24)));
    perseguidorNivel2Visual->setZValue(19);

    jugador.setX(80);
    jugador.setY(450);
    jugador.setVelocidadX(0);
    jugador.setVelocidadY(0);
    jugador.setEnSuelo(true);
    teclaIzquierdaPresionada = false;
    teclaDerechaPresionada = false;
    ataqueNivel2Activo = false;
    bajarPlataformaNivel2 = false;
    golpeJefeAplicadoAtaqueNivel2 = false;
    enemigosDerrotadosNivel2 = 0;
    tiempoRunnerNivel2 = 0.0f;
    fronteraIANivel2 = 0.0f;
    impulsoPersecucionNivel2 = 0.0f;
    objetivoJefeNivel2X = 0.0f;
    objetivoJefeNivel2Y = 0.0f;
    tiempoAtaqueJefeNivel2 = 0.0f;
    tiempoRafagaJefeNivel2 = 0.0f;
    golpesJefeNivel2 = 0;
    siguienteRafagaJefeNivel2 = 10;
    proyectilesRafagaJefeNivel2 = 0;
    jefeReposicionandoNivel2 = false;
    jefeRafagaCentralNivel2 = false;

    int enemigosEnPlataformas = 0;
    for(const Plataforma& plataformaNivel2 : plataformas)
    {
        if(plataformaNivel2.getAlto() >= 70.0f ||
            plataformaNivel2.getX() < 700.0f ||
            plataformaNivel2.getX() >= inicioArenaFinalNivel2 - 420.0f)
        {
            continue;
        }

        if(enemigosEnPlataformas % 3 == 0)
        {
            const float xEnemigo = plataformaNivel2.getX() + plataformaNivel2.getAncho() * 0.45f;
            Enemigo patrullero(xEnemigo,
                               plataformaNivel2.getY() - 58.0f,
                               PATRULLERO,
                               32,
                               dificultad == 2 ? 13 : 10);
            patrullero.setLimites(plataformaNivel2.getX() + 18.0f,
                                  plataformaNivel2.getX() + plataformaNivel2.getAncho() - 56.0f);
            patrullero.setVelocidades(1.35f + (dificultad == 2 ? 0.25f : 0.0f), 4.0f);
            enemigosNivel2.push_back(std::move(patrullero));
        }
        else if(enemigosEnPlataformas % 5 == 2)
        {
            Enemigo volador(plataformaNivel2.getX() + plataformaNivel2.getAncho() * 0.55f,
                            plataformaNivel2.getY() - 100.0f,
                            VOLADOR,
                            24,
                            dificultad == 2 ? 15 : 12);
            volador.setRangoDeteccion(260.0f);
            volador.setVelocidades(0.0f, 4.4f + (dificultad == 2 ? 0.35f : 0.0f));
            enemigosNivel2.push_back(std::move(volador));
        }

        enemigosEnPlataformas++;
    }

    jugadorVisual = escena->addPixmap(spriteQuieto.scaled(80, 80, Qt::KeepAspectRatio,
                                                          Qt::SmoothTransformation));
    jugadorVisual->setZValue(10);
    jugadorVisual->setPos(jugador.getX(), jugador.getY());

    vidaVisual = escena->addPixmap(vidaFull.scaled(115, 45, Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
    vidaVisual->setZValue(1000);
    crearCuraciones();
    crearTextoNivel2();
    actualizarVidaVisual();
    actualizarMovimientoHorizontal();
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

    int cantidadAplicada = cantidad;
    if(nivelJuego == 2)
        cantidadAplicada = std::max(1, static_cast<int>(std::ceil(cantidad / 3.0f)));

    vidaJugador = std::max(0, vidaJugador - cantidadAplicada);
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
    bajarPlataformaNivel2 = false;
    potenciadorActivo = false;
    tipoImpacto = 0;
    estabaCayendo = false;
    teclaIzquierdaPresionada = false;
    teclaDerechaPresionada = false;
    ataqueNivel2Activo = false;
    golpeJefeAplicadoAtaqueNivel2 = false;

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

    std::vector<int> indices;

    if(nivelJuego == 2)
    {
        for(int i = 0; i < static_cast<int>(plataformas.size()); ++i)
        {
            const Plataforma& plataforma = plataformas[i];
            if(plataforma.getAlto() < 70.0f &&
                plataforma.getX() > 850.0f &&
                plataforma.getX() < inicioArenaFinalNivel2 - 500.0f &&
                i % 7 == 0)
            {
                indices.push_back(i);
            }

            if(indices.size() >= 6)
                break;
        }
    }
    else
    {
        indices = {
            std::min(14, static_cast<int>(plataformas.size()) - 1),
            std::min(44, static_cast<int>(plataformas.size()) - 1)
        };
    }

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
    potenciadorActivo = true;
    tiempoPotenciador.restart();
    actualizarMovimientoHorizontal();

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
        jugador.desactivarVelocidad();
        potenciadorActivo = false;
        actualizarMovimientoHorizontal();

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

    if(textoNivel)
        textoNivel->setPos(zonaVisible.left() + 20, zonaVisible.top() + 72);

    if(textoPuntaje)
        textoPuntaje->setPos(zonaVisible.left() + 20, zonaVisible.top() + 102);

    if(textoTiempo)
        textoTiempo->setPos(zonaVisible.left() + 20, zonaVisible.top() + 126);
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
        const bool atravesandoPlataformaNivel2 = nivelJuego == 2 &&
                                                 bajarPlataformaNivel2 &&
                                                 p.getAlto() < 70.0f;

        if(colX && colY && jugador.getVelocidadY() >= 0 && !atravesandoPlataformaNivel2)
        {
            if(estabaCayendo)
            {
                if(nivelJuego != 2)
                {
                    const float distanciaCaida = p.getY() - inicioCaidaY;
                    if(distanciaCaida >= 700)
                        recibirDanio(100);
                    else if(distanciaCaida >= 400)
                        recibirDanio(50);
                    else if(distanciaCaida >= 200)
                        recibirDanio(25);
                }

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

    if(nivelJuego == 2)
        actualizarNivel2();

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
        if(nivelJuego != 2)
            recibirDanio(50);

        jugador.setY(limiteMundo.bottom() - 50);
        jugador.setVelocidadY(0);
        jugador.setEnSuelo(true);
        estabaCayendo = false;
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

    if(meta)
    {
        QRectF jugadorRect(jugador.getX(), jugador.getY(), 50, 50);

        if(nivelJuego == 1 && jugadorRect.intersects(meta->sceneBoundingRect()))
        {
            cambiarANivel2();
            centerOn(jugador.getX() + 25, jugador.getY() + 25);
            actualizarHud();
            return;
        }

        if(nivelJuego == 2 &&
            faseNivel2 == FaseNivel2::Runner &&
            nivelRunner.seAlcanzoLaMeta(jugador.getX()) &&
            jugadorRect.intersects(meta->sceneBoundingRect()))
        {
            iniciarDueloFinalNivel2();
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
    const bool accionesBloqueadasNivel2 = nivelJuego == 2 && protegiendo;

    if(event->key() == Qt::Key_A)
    {
        if(accionesBloqueadasNivel2)
            return;
        teclaIzquierdaPresionada = true;
        actualizarMovimientoHorizontal();
    }

    if(event->key() == Qt::Key_D)
    {
        if(accionesBloqueadasNivel2)
            return;
        teclaDerechaPresionada = true;
        actualizarMovimientoHorizontal();
    }

    if(event->key() == Qt::Key_S)
    {
        if(accionesBloqueadasNivel2)
            return;

        if(nivelJuego == 2)
        {
            bajarPlataformaNivel2 = true;
            if(jugador.getEnSuelo())
            {
                jugador.setY(jugador.getY() + 8.0f);
                jugador.setEnSuelo(false);
            }
        }
        else
        {
            protegiendo = true;
        }
    }

    if(event->key() == Qt::Key_Shift && nivelJuego == 2)
    {
        protegiendo = true;
        bajarPlataformaNivel2 = false;
        ataqueNivel2Activo = false;
        if(ataqueNivel2Visual)
            ataqueNivel2Visual->setVisible(false);
        actualizarMovimientoHorizontal();
    }

    if(event->key() == Qt::Key_J)
        iniciarAtaqueNivel2();

    if(event->key() == Qt::Key_Space)
    {
        if(accionesBloqueadasNivel2)
            return;

        if(nivelJuego == 2 && jugador.getEnSuelo())
        {
            jugador.setVelocidadY(jugador.getVelocidadExtra() ? -14.0f : -12.8f);
            jugador.setEnSuelo(false);
        }
        else
        {
            jugador.saltar();
        }
    }
}

void EscenaJuego::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)
        teclaIzquierdaPresionada = false;

    if(event->key() == Qt::Key_D)
        teclaDerechaPresionada = false;

    if(event->key() == Qt::Key_A || event->key() == Qt::Key_D)
        actualizarMovimientoHorizontal();

    if(event->key() == Qt::Key_S)
    {
        if(nivelJuego == 2)
            bajarPlataformaNivel2 = false;
        else
            protegiendo = false;
    }

    if(event->key() == Qt::Key_Shift && nivelJuego == 2)
    {
        protegiendo = false;
        actualizarMovimientoHorizontal();
    }
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