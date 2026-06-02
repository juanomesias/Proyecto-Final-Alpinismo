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

private:

    QGraphicsScene* escena;

    QGraphicsPixmapItem* fondoVisual;

    int etapa;

    QGraphicsPixmapItem* jugadorVisual;

    QPixmap spriteQuieto;
    QPixmap spriteCorrer;
    QPixmap spriteSaltar;

    QTimer* timer;

    Jugador jugador;

    MovimientoParabolico gravedad;
    EfectoFriccion friccion;

    std::vector<Plataforma> plataformas;

    std::vector<Obstaculo> obstaculos;

    std::vector<QGraphicsRectItem*> obstaculosVisuales;

    std::vector<PowerUp> powerups;

    std::vector<QGraphicsEllipseItem*> powerupsVisuales;

    QGraphicsRectItem* meta;

    QGraphicsTextItem* textoVida;
    QGraphicsTextItem* textoNivel;
    QGraphicsTextItem* textoPuntaje;
    QGraphicsTextItem* textoTiempo;

    int tiempoJuego;

public:

    EscenaJuego(QWidget *parent = nullptr);

    ~EscenaJuego();

    void actualizarJuego();

protected:

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif