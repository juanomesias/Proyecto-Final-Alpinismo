#ifndef MOVIMIENTOOSCILATORIO_H
#define MOVIMIENTOOSCILATORIO_H

#include "fisica.h"

class MovimientoOscilatorio : public Fisica
{
private:
    float amplitud;

    float frecuencia;

    float tiempo;

public:
    MovimientoOscilatorio(float amplitud = 1.5f,
                          float frecuencia = 0.05f);

    void aplicar(Entidad* entidad) override;

    void actualizarTiempo(float deltaTiempo);
};

#endif // MOVIMIENTOOSCILATORIO_H