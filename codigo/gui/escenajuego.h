#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTimer>
#include <QKeyEvent>

#include "../logica/jugador.h"

class EscenaJuego : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene* escena;

    QGraphicsRectItem* jugadorVisual;

    QTimer* timer;

    Jugador jugador;

public:
    EscenaJuego(QWidget *parent = nullptr);

    ~EscenaJuego();

    void actualizarJuego();

protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // ESCENAJUEGO_H