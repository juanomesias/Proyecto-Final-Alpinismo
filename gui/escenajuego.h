#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <QKeyEvent>
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
    float limiteNivel;

    QGraphicsPixmapItem* jugadorVisual;

    // Construye todo el mundo de una sola vez
    void construirMundo(int mundoAlto);
    void cambiarFondoPorEtapa();

    // Métodos que siguen existiendo para no romper compilación
    void cargarNivel(int id, float offsetY = 0);
    void cambiarNivel();
    void limpiarPlataformas();
    void limpiarObstaculos();
    void agregarPlataforma(float x, float y, float ancho, float alto);

    QPixmap spriteQuieto;
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;
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

    QGraphicsRectItem*  meta       = nullptr;
    QGraphicsTextItem*  textoVida;
    bool bloqueoCambio = false;
    bool cambioEscenaRealizado = false;
    QGraphicsTextItem*  textoNivel;
    QGraphicsTextItem*  textoPuntaje;
    QGraphicsTextItem*  textoTiempo;
    int tiempoJuego;

    static const int ALTO_PANTALLA = 600;
    static const int NUM_FONDOS    = 10;

public:
    EscenaJuego(QWidget *parent = nullptr);
    ~EscenaJuego();

    void actualizarJuego();
    void cargarEscalada(int nivel);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif