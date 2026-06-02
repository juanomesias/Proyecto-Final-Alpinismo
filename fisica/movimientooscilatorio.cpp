#include "movimientooscilatorio.h"
#include <cmath>

MovimientoOscilatorio::
    MovimientoOscilatorio(
        float amplitud)
{
    this->amplitud = amplitud;
    tiempo = 0;
}

float MovimientoOscilatorio::
    calcularY(float yBase)
{
    tiempo += 0.1f;

    return yBase +
           sin(tiempo)
               * amplitud;
}