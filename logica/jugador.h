#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"

class Jugador : public Entidad
{
private:
    int vida;
    int puntaje;

    bool tieneArmadura;
    bool enSuelo;

    float fuerzaSalto;

public:
    Jugador(float xInicial = 0, float yInicial = 0);
    void actualizar() override;

    void saltar();

    void recibirDanio(int cantidad);

    void obtenerArmadura();

    int getVida() const;

    bool getTieneArmadura() const;

    int getPuntaje() const;

    float getFuerzaSalto() const;

    bool getEnSuelo() const;

    void setVida(int nuevaVida);

    void setPuntaje(int nuevoPuntaje);

    void setEnSuelo(bool estado);
};

#endif // JUGADOR_H
