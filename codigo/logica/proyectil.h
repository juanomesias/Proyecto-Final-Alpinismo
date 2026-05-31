#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "entidad.h"

class Proyectil : public Entidad
{
private:
    float direccionX;
    float direccionY;
    int danio;

    bool activo;

public:
    Proyectil(float xInicial,
              float yInicial,
              float direccionX,
              float direccionY,
              float velocidad,
              int danio);

    void actualizar() override;

    int getDanio() const;

    bool estaActivo() const;

    void desactivar();
};

#endif // PROYECTIL_H