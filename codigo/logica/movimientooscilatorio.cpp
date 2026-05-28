#include "movimientooscilatorio.h"

#include <cmath>

MovimientoOscilatorio::MovimientoOscilatorio(float amplitud,
                                             float frecuencia)
    : Fisica(amplitud)
{
    this->amplitud = amplitud;

    this->frecuencia = frecuencia;

    tiempo = 0;
}

void MovimientoOscilatorio::aplicar(Entidad* entidad)
{
    float fuerza =
        amplitud * sin(frecuencia * tiempo);

    entidad->setVelocidadX(
        entidad->getVelocidadX() + fuerza
        );
}

void MovimientoOscilatorio::actualizarTiempo(float deltaTiempo)
{
    tiempo += deltaTiempo;
}