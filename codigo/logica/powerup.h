#ifndef POWERUP_H
#define POWERUP_H

#include "entidad.h"

enum TipoPowerUp
{
    ARMADURA,
    VIDA,
    PUNTAJE
};

class PowerUp : public Entidad
{
private:
    TipoPowerUp tipo;

    bool recogido;

    float ancho;
    float alto;

public:
    PowerUp(float xInicial,
            float yInicial,
            TipoPowerUp tipo);

    void actualizar() override;

    TipoPowerUp getTipo() const;

    bool fueRecogido() const;

    void recoger();

    float getAncho() const;

    float getAlto() const;
};

#endif // POWERUP_H