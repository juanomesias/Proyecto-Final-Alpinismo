#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QPixmap>
#include <QString>
#include <QVector>

#include "../logica/jugador.h"
#include "../logica/plataforma.h"
#include "../logica/obstaculo.h"
#include "../logica/powerup.h"
#include "../logica/meta.h"
#include "../fisica/movimientoparabolico.h"
#include "../fisica/efectofriccion.h"
#include <vector>

class QAudioOutput;
class QMediaPlayer;

class EscenaJuego : public QGraphicsView
{
    Q_OBJECT
    QVector<QGraphicsPixmapItem*> fondosItems;

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
    QTimer* timer;

    Jugador              jugador;
    MovimientoParabolico gravedad;
    EfectoFriccion       friccion;

    std::vector<Plataforma>            plataformas;
    std::vector<Obstaculo>             obstaculos;
    std::vector<QGraphicsRectItem*>    obstaculosVisuales;
    std::vector<PowerUp>               powerups;
    std::vector<QGraphicsEllipseItem*> powerupsVisuales;
    std::vector<QGraphicsPixmapItem*>  plataformasVisuales;
    std::vector<QGraphicsPixmapItem*>  piedrasVisuales;
    std::vector<float>                 velocidadesPiedras;
    std::vector<QPixmap>               spritesPiedras;
    std::vector<QGraphicsPixmapItem*>  curacionesVisuales;
    std::vector<QPixmap>               spritesReloj;
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
    QGraphicsTextItem*  textoVida;
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
    QGraphicsTextItem*  textoNivel;
    QGraphicsTextItem*  textoPuntaje;
    QGraphicsTextItem*  textoTiempo;
    int tiempoJuego;

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
