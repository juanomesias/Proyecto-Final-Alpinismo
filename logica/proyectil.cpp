#include "proyectil.h"

Proyectil::Proyectil(float xInicial,
                     float yInicial,
                     float direccionX,
                     float direccionY,
                     float velocidad,
                     int danio)
    : Entidad(xInicial, yInicial)
{
    this->direccionX = direccionX;
    this->direccionY = direccionY;
    this->danio = danio;
    activo = true;

    velocidadX = direccionX * velocidad;
    velocidadY = direccionY * velocidad;
}

void Proyectil::actualizar()
{
    mover();
}

int Proyectil::getDanio() const
{
    return danio;
}

bool Proyectil::estaActivo() const
{
    return activo;
}

float Proyectil::getDireccionX() const
{
    return direccionX;
}

float Proyectil::getDireccionY() const
{
    return direccionY;
}

void Proyectil::desactivar()
{
    activo = false;
}