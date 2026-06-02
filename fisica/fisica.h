#ifndef FISICA_H
#define FISICA_H
#include "../logica/entidad.h"

class Fisica
{
protected:
    float intensidad;
public:
    Fisica(float intensidad = 0);

    virtual ~Fisica();

    virtual void aplicar(Entidad* entidad) = 0;
};

#endif // FISICA_H
