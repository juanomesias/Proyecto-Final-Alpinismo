#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QColor>
#include <QPixmap>
#include <QString>
#include <QVector>

#include "../logica/jugador.h"
#include "../logica/plataforma.h"
#include "../logica/obstaculo.h"
#include "../logica/powerup.h"
#include "../logica/meta.h"
#include "../logica/enemigo.h"
#include "../logica/nivelrunner.h"
#include "../logica/nivelplataforma.h"
#include "../fisica/movimientoparabolico.h"
#include "../fisica/movimientooscilatorio.h"
#include "../fisica/efectofriccion.h"
#include <vector>

class QAudioOutput;
class QMediaPlayer;

class EscenaJuego : public QGraphicsView
{
    Q_OBJECT
    QVector<QGraphicsPixmapItem*> fondosItems;

    enum class FaseNivel2
    {
        Runner,
        DueloFinal,
        Completado
    };

    struct OndaOscilatoriaNivel2
    {
        float x;
        float y;
        float yBase;
        float velocidadX;
        float ancho;
        float alto;
        int danio;
        bool activa;
        MovimientoOscilatorio movimiento;

        OndaOscilatoriaNivel2(float xInicial,
                              float yBaseInicial,
                              float velocidadXInicial,
                              float amplitud,
                              float anchoInicial,
                              float altoInicial,
                              int danioInicial)
            : x(xInicial),
            y(yBaseInicial),
            yBase(yBaseInicial),
            velocidadX(velocidadXInicial),
            ancho(anchoInicial),
            alto(altoInicial),
            danio(danioInicial),
            activa(true),
            movimiento(amplitud)
        {
        }
    };

private:
    QGraphicsScene*       escena;

    int   etapa;
    int   subEtapa;
    int   alturaEscalada;
    int   nivelActual;
    int   dificultad;
    int   danioRoca;
    int   nivelJuego;
    float limiteNivel;

    QGraphicsPixmapItem* jugadorVisual;

    // Construye todo el mundo de una sola vez
    void construirMundo(int mundoAlto);
    void construirNivel2();
    void cambiarFondoPorEtapa();

    // Métodos que siguen existiendo para no romper compilación
    void cargarNivel(int id, float offsetY = 0);
    void cambiarNivel();
    void limpiarPlataformas();
    void limpiarObstaculos();
    void agregarPlataforma(float x, float y, float ancho, float alto);
    void agregarPlataformaResbalosa(float x, float y, float ancho, float alto);
    void agregarPiso(float x, float y, float ancho, float alto);
    void recibirDanio(int cantidad);
    void actualizarVidaVisual();
    void crearPiedras();
    void actualizarPiedras();
    void reiniciarPiedra(int indice);
    void crearAyuda();
    void actualizarAyuda();
    void reiniciarAyuda();
    void curarJugador(int cantidad);
    void recibirImpactoObjeto(int cantidad);
    void mostrarImpacto(bool conEscudo);
    void crearCuraciones();
    void actualizarCuraciones();
    void activarPotenciador();
    void actualizarPotenciador();
    void cargarSpritesReloj();
    void configurarAudio();
    void reproducirMusicaNivel1();
    void reproducirMusicaNivel2();
    void reproducirSonidoDanio();
    void reproducirSonidoMuerte();
    void cambiarANivel2();
    void actualizarHud();
    void mostrarPantallaMuerte();
    void reiniciarNivelActual();
    void usarSpritesNivel1();
    void usarSpritesNivel2();
    void actualizarMovimientoHorizontal();
    void actualizarNivel2();
    void sincronizarVisualesEnemigosNivel2();
    void sincronizarProyectilesEnemigo(const Enemigo& enemigo,
                                       std::vector<QGraphicsPixmapItem*>& visuales,
                                       const QColor& color);
    void actualizarOndasOscilatoriasNivel2();
    void lanzarOndaOscilatoriaNivel2(const Enemigo& origen,
                                     float amplitud,
                                     float velocidad,
                                     int danio);
    void invocarApoyoIANivel2();
    void iniciarDueloFinalNivel2();
    void actualizarDueloFinalNivel2(float deltaTiempo, bool jugadorEnAireAhora);
    void elegirNuevaPosicionJefeNivel2();
    void lanzarRafagaCentralJefeNivel2();
    void iniciarAtaqueAltoJefeNivel2();
    void actualizarAtaqueAltoJefeNivel2(float deltaTiempo);
    void limpiarEntidadesSecundariasNivel2();
    QPixmap crearSpriteMuroNivel2() const;
    void actualizarAtaqueNivel2();
    void iniciarAtaqueNivel2();
    QRectF obtenerAreaAtaqueNivel2() const;
    void resolverAtaqueNivel2();
    void crearTextoNivel2();

    QPixmap spriteNivel1Quieto;
    QPixmap spriteNivel1Saltar;
    QPixmap spriteNivel1Cayendo;
    QPixmap spriteNivel1QuietoReves;
    QPixmap spriteNivel1SaltarReves;
    QPixmap spriteNivel1CayendoReves;
    QPixmap spriteNivel2Quieto;
    QPixmap spriteNivel2Saltar;
    QPixmap spriteNivel2Cayendo;
    QPixmap spriteQuieto;
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
    QPixmap spriteCayendo;
    QPixmap spriteQuietoReves;
    QPixmap spriteSaltarReves;
    QPixmap spriteCayendoReves;
    QPixmap spriteMuerto;
    QPixmap spriteProteccion;
    QPixmap spriteGolpe;
    QPixmap spriteGolpeEscudo;
    QPixmap spritePortal;
    QPixmap spritePlataformaNieve;
    QPixmap spritePiso;
    QPixmap spritePiso2;
    QPixmap spriteAyuda;
    QPixmap spriteCuracion;
    QPixmap spriteWasted;
    QPixmap spriteBotonRepetir;
    QPixmap spriteBotonSalir;
    QPixmap spritePortalGuia;
    QPixmap spritePlataformaHielo;
    QPixmap spriteAtaqueNivel2;
    QPixmap spriteAgachadoEscudo;
    QPixmap spriteGolpeAgachado;
    QPixmap spriteDanioNivel2;
    QPixmap spriteDragonNivel2;
    QPixmap spriteAtaqueDragon;
    QPixmap spriteAtaqueDragonReves;
    QPixmap spriteFantasmaFinalNivel2;
    QPixmap spriteAtaqueFinalNivel2;
    QPixmap spriteAtaqueFinalNivel2Reves;
    QPixmap spritePatrulleroNivel2;
    QPixmap spriteVigilanteNivel2;
    QPixmap spriteObstaculoNivel2;
    QPixmap spriteMuroNivel2;
    QTimer* timer;

    Jugador              jugador;
    MovimientoParabolico gravedad;
    EfectoFriccion       friccion;

    std::vector<Plataforma>            plataformas;
    std::vector<bool>                  plataformasResbalosas;
    std::vector<Obstaculo>             obstaculos;
    std::vector<QGraphicsPixmapItem*>  obstaculosVisuales;
    std::vector<PowerUp>               powerups;
    std::vector<QGraphicsEllipseItem*> powerupsVisuales;
    std::vector<QGraphicsPixmapItem*>  plataformasVisuales;
    std::vector<QGraphicsPixmapItem*>  piedrasVisuales;
    std::vector<float>                 velocidadesPiedras;
    std::vector<QPixmap>               spritesPiedras;
    std::vector<QGraphicsPixmapItem*>  curacionesVisuales;
    std::vector<QPixmap>               spritesReloj;
    NivelRunner nivelRunner;
    Enemigo perseguidorNivel2;
    std::vector<Enemigo>               enemigosNivel2;
    std::vector<QGraphicsPixmapItem*>  enemigosNivel2Visuales;
    std::vector<std::vector<QGraphicsPixmapItem*>> proyectilesEnemigosNivel2Visuales;
    std::vector<QGraphicsPixmapItem*>  proyectilesPerseguidorVisuales;
    std::vector<OndaOscilatoriaNivel2> ondasOscilatoriasNivel2;
    std::vector<QGraphicsPixmapItem*>  ondasOscilatoriasNivel2Visuales;
    QGraphicsPixmapItem* perseguidorNivel2Visual = nullptr;
    QGraphicsRectItem* barreraIAVisual = nullptr;
    QGraphicsPixmapItem* barreraMuroNivel2Visual = nullptr;
    QGraphicsPixmapItem* ataqueNivel2Visual = nullptr;
    QGraphicsRectItem* muroArenaIzquierdoNivel2Visual = nullptr;
    QGraphicsRectItem* muroArenaDerechoNivel2Visual = nullptr;
    QGraphicsPixmapItem* ayudaVisual = nullptr;
    QGraphicsPixmapItem* relojVisual = nullptr;
    QGraphicsPixmapItem* wastedVisual = nullptr;
    QGraphicsPixmapItem* botonRepetirVisual = nullptr;
    QGraphicsPixmapItem* botonSalirVisual = nullptr;
    float velocidadAyuda = 2.5f;
    QElapsedTimer tiempoPotenciador;
    QElapsedTimer tiempoImpacto;
    bool potenciadorActivo = false;
    bool protegiendo = false;
    int tipoImpacto = 0;

    QGraphicsPixmapItem* meta       = nullptr;
    QGraphicsPixmapItem* vidaVisual = nullptr;
    QGraphicsTextItem*  textoVida = nullptr;
    QPixmap vidaFull;
    QPixmap vidaUnoMenos;
    QPixmap vidaMitad;
    QPixmap vidaPoca;
    QPixmap vidaSin;
    int vidaJugador = 100;
    float inicioCaidaY = 0;
    bool estabaCayendo = false;
    bool juegoTerminado = false;
    bool mirandoIzquierda = false;
    bool bloqueoCambio = false;
    bool cambioEscenaRealizado = false;
    bool teclaIzquierdaPresionada = false;
    bool teclaDerechaPresionada = false;
    bool ataqueNivel2Activo = false;
    bool inmunidadInicioNivel2 = false;
    bool bajarPlataformaNivel2 = false;
    bool golpeJefeAplicadoAtaqueNivel2 = false;
    FaseNivel2 faseNivel2 = FaseNivel2::Runner;
    QGraphicsTextItem*  textoNivel = nullptr;
    QGraphicsTextItem*  textoPuntaje = nullptr;
    QGraphicsTextItem*  textoTiempo = nullptr;
    int tiempoJuego = 0;
    int enemigosDerrotadosNivel2 = 0;
    float tiempoRunnerNivel2 = 0.0f;
    float fronteraIANivel2 = 0.0f;
    float impulsoPersecucionNivel2 = 0.0f;
    float inicioArenaFinalNivel2 = 0.0f;
    float finArenaFinalNivel2 = 0.0f;
    float objetivoJefeNivel2X = 0.0f;
    float objetivoJefeNivel2Y = 0.0f;
    float tiempoAtaqueJefeNivel2 = 0.0f;
    float tiempoRafagaJefeNivel2 = 0.0f;
    float tiempoAtaqueAltoJefeNivel2 = 0.0f;
    float duracionAtaqueAltoJefeNivel2 = 0.0f;
    float tiempoDisparoAltoJefeNivel2 = 0.0f;
    int golpesJefeNivel2 = 0;
    int siguienteRafagaJefeNivel2 = 10;
    int proyectilesRafagaJefeNivel2 = 0;
    int proyectilesAtaqueAltoJefeNivel2 = 0;
    bool jefeReposicionandoNivel2 = false;
    bool jefeRafagaCentralNivel2 = false;
    bool jefeAtaqueAltoNivel2 = false;
    QElapsedTimer tiempoAtaqueNivel2;
    QElapsedTimer tiempoDanioNivel2;
    QElapsedTimer tiempoInvulnerabilidadJugador;

    QMediaPlayer* musica = nullptr;
    QAudioOutput* salidaMusica = nullptr;
    QMediaPlayer* sonidoDanio = nullptr;
    QAudioOutput* salidaDanio = nullptr;
    QMediaPlayer* sonidoMuerte = nullptr;
    QAudioOutput* salidaMuerte = nullptr;

    static const int ALTO_PANTALLA = 600;
    static const int NUM_FONDOS    = 10;

public:
    EscenaJuego(QWidget *parent = nullptr, int dificultad = 1);
    ~EscenaJuego();

    void actualizarJuego();
    void cargarEscalada(int nivel);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
