#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"

class jugador : public Entidad
{
private:
    int vida;

    bool tieneArmadura;

    int puntaje;

    float fuerzaSalto;

public:
    jugador(float xInicial = 0, float yInicial = 0);
    void actualizar() override;

    void saltar();

    void recibirDanio(int cantidad);

    void obtenerArmadura();

    int getVida() const;

    bool getTieneArmadura() const;

    int getPuntaje() const;

    float getFuerzaSalto() const;

    void setVida(int nuevaVida);

    void setPuntaje(int nuevoPuntaje);
};

#endif // JUGADOR_H
