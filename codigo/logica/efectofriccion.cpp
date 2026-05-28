#include "efectofriccion.h"

EfectoFriccion::EfectoFriccion(float friccion)
    : Fisica(friccion)
{
    this->friccion = friccion;
}

void EfectoFriccion::aplicar(Entidad* entidad)
{
    float velocidadX = entidad->getVelocidadX();

    // Frenar hacia la derecha
    if(velocidadX > 0)
    {
        velocidadX -= friccion;

        if(velocidadX < 0)
        {
            velocidadX = 0;
        }
    }

    // Frenar hacia la izquierda
    else if(velocidadX < 0)
    {
        velocidadX += friccion;

        if(velocidadX > 0)
        {
            velocidadX = 0;
        }
    }

    entidad->setVelocidadX(velocidadX);
}