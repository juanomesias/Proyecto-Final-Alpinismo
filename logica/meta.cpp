#include "meta.h"

Meta::Meta(
    float x,
    float y,
    float ancho,
    float alto
    )
    : Entidad(x,y)
{
    this->ancho = ancho;
    this->alto = alto;
}

float Meta::getAncho() const
{
    return ancho;
}

float Meta::getAlto() const
{
    return alto;
}