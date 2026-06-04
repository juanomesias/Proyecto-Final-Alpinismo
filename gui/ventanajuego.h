#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QMainWindow>

class EscenaJuego;
class VentanaJuego : public QMainWindow
{
    Q_OBJECT
private:
    EscenaJuego* escena;

public:
    VentanaJuego(QWidget *parent = nullptr, int dificultad = 1);

    ~VentanaJuego();
};

#endif // VENTANAJUEGO_H
