#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>

#include "../logica/jugador.h"
#include "../logica/movimientoparabolico.h"
#include "../logica/plataforma.h"
#include "../logica/obstaculo.h"
#include <vector>

class EscenaJuego : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene* escena;
    QGraphicsRectItem* jugadorVisual;
    QTimer* timer;

    Jugador jugador;
    MovimientoParabolico gravedad;

    std::vector<Plataforma> plataformas;

    std::vector<Obstaculo> obstaculos;

    std::vector<QGraphicsRectItem*> obstaculosVisuales;

public:
    EscenaJuego(QWidget *parent = nullptr);

    ~EscenaJuego();

    void actualizarJuego();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // ESCENAJUEGO_H