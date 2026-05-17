#ifndef MOVIMIENTOPARABOLICO_H
#define MOVIMIENTOPARABOLICO_H
#include "fisica.h"

class MovimientoParabolico : public Fisica
{
private:
    float gravedad;
public:
    MovimientoParabolico(float gravedad = 0.5f);

    void aplicar(Entidad* entidad) override;
};

#endif // MOVIMIENTOPARABOLICO_H
