#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include <QMainWindow>

class QPushButton;
class QLabel;

class MenuPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuPrincipal(QWidget *parent = nullptr);

private:
    QLabel *titulo;
    QPushButton *botonIniciar;
    QPushButton *botonFacil;
    QPushButton *botonDificil;

    int dificultad;
};

#endif