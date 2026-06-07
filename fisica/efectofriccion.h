#ifndef EFECTOFRICCION_H
#define EFECTOFRICCION_H

#include "fisica.h"

class EfectoFriccion : public Fisica
{
public:
    EfectoFriccion(float desaceleracion = 0.12f);

    void aplicar(Entidad* entidad) override;

    void aplicarSinFriccion(Entidad* entidad);
};

#endif
