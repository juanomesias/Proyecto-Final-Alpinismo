#include "efectofriccion.h"

EfectoFriccion::EfectoFriccion(float desaceleracion)
    : Fisica(desaceleracion)
{
}

void EfectoFriccion::aplicar(Entidad* entidad)
{
    if(!entidad)
        return;

    float velocidad = entidad->getVelocidadX();

    if(velocidad > 0.0f)
    {
        velocidad -= intensidad;
        if(velocidad < 0.0f)
            velocidad = 0.0f;
    }
    else if(velocidad < 0.0f)
    {
        velocidad += intensidad;
        if(velocidad > 0.0f)
            velocidad = 0.0f;
    }

    entidad->setVelocidadX(velocidad);
}

void EfectoFriccion::aplicarSinFriccion(Entidad* entidad)
{
    aplicar(entidad);
}
