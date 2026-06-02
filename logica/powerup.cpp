#include "powerup.h"

PowerUp::PowerUp(
    float x,
    float y,
    TipoPowerUp tipo)
    : Entidad(x,y)
{
    this->tipo = tipo;
    activo = true;
}

TipoPowerUp PowerUp::getTipo() const
{
    return tipo;
}

bool PowerUp::estaActivo() const
{
    return activo;
}

void PowerUp::setActivo(bool estado)
{
    activo = estado;
}
