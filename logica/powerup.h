#ifndef POWERUP_H
#define POWERUP_H

#include "entidad.h"

enum TipoPowerUp
{
    VELOCIDAD,
    VIDA,
    EXPERIENCIA
};

class PowerUp : public Entidad
{
private:
    TipoPowerUp tipo;
    bool activo;

public:
    PowerUp(float x,
            float y,
            TipoPowerUp tipo);

    TipoPowerUp getTipo() const;

    bool estaActivo() const;

    void setActivo(bool estado);
};

#endif