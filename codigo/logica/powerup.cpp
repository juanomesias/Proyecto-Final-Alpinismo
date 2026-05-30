#include "powerup.h"

PowerUp::PowerUp(float xInicial,
                 float yInicial,
                 TipoPowerUp tipo)
    : Entidad(xInicial, yInicial)
{
    this->tipo = tipo;

    recogido = false;

    ancho = 40;
    alto = 40;
}

void PowerUp::actualizar()
{
    // Por ahora no hace nada.
}

TipoPowerUp PowerUp::getTipo() const
{
    return tipo;
}

bool PowerUp::fueRecogido() const
{
    return recogido;
}

void PowerUp::recoger()
{
    recogido = true;

    setActiva(false);
}

float PowerUp::getAncho() const
{
    return ancho;
}

float PowerUp::getAlto() const
{
    return alto;
}