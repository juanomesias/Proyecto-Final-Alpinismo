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