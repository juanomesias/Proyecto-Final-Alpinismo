#ifndef JUGADOR_H
#define JUGADOR_H

#include "entidad.h"

class Jugador : public Entidad
{
private:
    int vida;
    int puntaje;

    int nivel;
    int experiencia;

    bool tieneArmadura;
    bool enSuelo;
    bool velocidadExtra;

    float fuerzaSalto;

    float velocidadNormal;
    float velocidadActual;

public:
    Jugador(float xInicial = 0, float yInicial = 0);

    void actualizar() override;

    void saltar();

    void recibirDanio(int cantidad);

    void obtenerArmadura();

    void ganarExperiencia(int cantidad);

    void activarVelocidad();

    void desactivarVelocidad();

    int getVida() const;

    bool getTieneArmadura() const;

    int getPuntaje() const;

    int getNivel() const;

    int getExperiencia() const;

    float getFuerzaSalto() const;

    bool getEnSuelo() const;

    bool getVelocidadExtra() const;

    float getVelocidadActual() const;

    void setVida(int nuevaVida);

    void setPuntaje(int nuevoPuntaje);

    void setEnSuelo(bool estado);
};

#endif