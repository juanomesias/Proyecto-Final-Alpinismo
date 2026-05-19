#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "entidad.h"

class Obstaculo : public Entidad
{
private:
    int danio;

    float ancho;

    float alto;

public:
    Obstaculo(float xInicial,
              float yInicial,
              int danio,
              float ancho,
              float alto);

    int getDanio() const;

    float getAncho() const;

    float getAlto() const;
};

#endif // OBSTACULO_H
