#ifndef NIVELPLATAFORMA_H
#define NIVELPLATAFORMA_H

#include "nivel.h"

class NivelPlataforma : public Nivel
{
private:
    float posicionInicialJugadorX;
    float posicionInicialJugadorY;

    float metaX;
    float metaY;

    bool armaduraObtenida;

public:
    NivelPlataforma();

    void actualizar() override;

    void cargarNivel();

    void obtenerArmadura();

    bool seAlcanzoLaMeta(float xJugador, float yJugador) const;

    float getPosicionInicialJugadorX() const;
    float getPosicionInicialJugadorY() const;

    float getMetaX() const;
    float getMetaY() const;

    bool getArmaduraObtenida() const;
};

#endif // NIVELPLATAFORMA_H