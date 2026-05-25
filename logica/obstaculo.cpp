#include "obstaculo.h"

Obstaculo::Obstaculo(float xInicial,
                     float yInicial,
                     int danio,
                     float ancho,
                     float alto)
    : Entidad(xInicial, yInicial)
{
    this->danio = danio;

    this->ancho = ancho;

    this->alto = alto;
}

int Obstaculo::getDanio() const
{
    return danio;
}

float Obstaculo::getAncho() const
{
    return ancho;
}

float Obstaculo::getAlto() const
{
    return alto;
}