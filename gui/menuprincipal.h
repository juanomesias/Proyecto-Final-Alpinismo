#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include <QMainWindow>
#include <QString>

class QPushButton;
class QLabel;
class QWidget;

class MenuPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuPrincipal(QWidget *parent = nullptr);

private:
    QWidget *central;
    QPushButton *botonIniciar;
    QPushButton *botonFacil;
    QPushButton *botonDificil;

    int dificultad;

    void mostrarInicio();
    void mostrarSeleccionDificultad();
    void iniciarJuego(int dificultadSeleccionada);
    void prepararBotonImagen(QPushButton *boton, const QString& ruta, int ancho, int alto);
};

#endif
