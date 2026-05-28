#ifndef EFECTOFRICCION_H
#define EFECTOFRICCION_H

#include "fisica.h"

class EfectoFriccion : public Fisica
{
private:
    float friccion;

public:
    EfectoFriccion(float friccion = 0.2f);

    void aplicar(Entidad* entidad) override;
};

#endif // EFECTOFRICCION_H