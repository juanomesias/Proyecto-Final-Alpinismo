#include "movimientoparabolico.h"

MovimientoParabolico::MovimientoParabolico(float gravedad)
    : Fisica(gravedad)
{
    this->gravedad = gravedad;
}

void MovimientoParabolico::aplicar(Entidad* entidad)
{
    entidad->setVelocidadY(
        entidad->getVelocidadY() + gravedad
        );
}
