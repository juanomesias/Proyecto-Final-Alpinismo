#include "entidad.h"

Entidad::Entidad(float xInicial, float yInicial)
{
    x = xInicial;
    y = yInicial;
    velocidadX = 0;
    velocidadY = 0;
    activa = true;
}

Entidad::~Entidad()
{
}

void Entidad::actualizar()
{
    mover();
}

void Entidad::mover()
{
    x += velocidadX;
    y += velocidadY;
}

// GETTERS
float Entidad::getX() const
{
    return x;
}

float Entidad::getY() const
{
    return y;
}

float Entidad::getVelocidadX() const
{
    return velocidadX;
}

float Entidad::getVelocidadY() const
{
    return velocidadY;
}

bool Entidad::estaActiva() const
{
    return activa;
}

// SETTERS
void Entidad::setX(float nuevaX)
{
    x = nuevaX;
}

void Entidad::setY(float nuevaY)
{
    y = nuevaY;
}

void Entidad::setVelocidadX(float nuevaVelocidadX)
{
    velocidadX = nuevaVelocidadX;
}

void Entidad::setVelocidadY(float nuevaVelocidadY)
{
    velocidadY = nuevaVelocidadY;
}

void Entidad::setActiva(bool estado)
{
    activa = estado;
}