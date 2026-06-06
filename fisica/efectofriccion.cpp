#include "efectofriccion.h"

EfectoFriccion::EfectoFriccion(float valor)
    : Fisica(valor)
{
}

void EfectoFriccion::aplicar(
    Entidad* entidad)
{
    entidad->setVelocidadX(
        entidad->getVelocidadX() * intensidad
        );
}

void EfectoFriccion::aplicarSinFriccion(
    Entidad* entidad,
    float impulsoMinimo)
{
    float velocidad = entidad->getVelocidadX();

    if(velocidad > 0.0f && velocidad < impulsoMinimo)
        velocidad = impulsoMinimo;
    else if(velocidad < 0.0f && velocidad > -impulsoMinimo)
        velocidad = -impulsoMinimo;
    else if(velocidad == 0.0f)
        velocidad = impulsoMinimo;
    else
        velocidad *= 1.015f;

    entidad->setVelocidadX(velocidad);
}
