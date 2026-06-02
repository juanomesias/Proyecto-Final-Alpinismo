#ifndef EFECTOFRICCION_H
#define EFECTOFRICCION_H

#include "fisica.h"

class EfectoFriccion : public Fisica
{
public:
    EfectoFriccion(float valor = 0.90f);

    void aplicar(
        Entidad* entidad
        ) override;
};

#endif
