#ifndef MOVIMIENTOOSCILATORIO_H
#define MOVIMIENTOOSCILATORIO_H

#include "fisica.h"

class MovimientoOscilatorio
{
private:
    float amplitud;
    float tiempo;

public:
    MovimientoOscilatorio(
        float amplitud = 10
        );

    float calcularY(float yBase);
};

#endif