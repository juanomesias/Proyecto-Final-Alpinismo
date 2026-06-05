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
    Proyectil(float xInicial = 0.0f,
              float yInicial = 0.0f,
              float direccionX = 1.0f,
              float direccionY = 0.0f,
              float velocidad = 0.0f,
              int danio = 10);

    void actualizar() override;

    int getDanio() const;
    bool estaActivo() const;
    float getDireccionX() const;
    float getDireccionY() const;

    void desactivar();
};

#endif // PROYECTIL_H
