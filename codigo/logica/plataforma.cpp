#include "plataforma.h"

Plataforma::Plataforma(float x,
                       float y,
                       float ancho,
                       float alto)
{
    this->x = x;
    this->y = y;
    this->ancho = ancho;
    this->alto = alto;
}

float Plataforma::getX() const
{
    return x;
}

float Plataforma::getY() const
{
    return y;
}

float Plataforma::getAncho() const
{
    return ancho;
}

float Plataforma::getAlto() const
{
    return alto;
}

void Plataforma::setX(float nuevaX)
{
    x = nuevaX;
}

void Plataforma::setY(float nuevaY)
{
    y = nuevaY;
}