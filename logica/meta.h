#ifndef META_H
#define META_H

#include "entidad.h"

class Meta : public Entidad
{
private:
    float ancho;
    float alto;

public:
    Meta(
        float x,
        float y,
        float ancho,
        float alto
        );

    float getAncho() const;
    float getAlto() const;
};

#endif