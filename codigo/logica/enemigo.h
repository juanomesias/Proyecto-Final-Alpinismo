#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "entidad.h"

enum TipoEnemigo
{
    PATRULLERO,
    VOLADOR,
    JEFE
};

enum EstadoEnemigo
{
    QUIETO,
    PATRULLANDO,
    EMBISTIENDO,
    RECUPERANDO
};

class Enemigo : public Entidad
{
private:
    TipoEnemigo tipo;
    EstadoEnemigo estado;

    int vida;
    int danio;

    float velocidadMovimiento;
    float velocidadEmbiste;

    float limiteIzquierdo;
    float limiteDerecho;

    int direccion;

    float rangoDeteccion;

    float tiempoEstado;

    float objetivoX;
    float objetivoY;

public:
    Enemigo(float xInicial = 0,
            float yInicial = 0,
            TipoEnemigo tipo = PATRULLERO,
            int vida = 30,
            int danio = 10);

    void actualizar() override;

    void detectarJugador(float xJugador, float yJugador);

    void recibirDanio(int cantidad);

    void setLimites(float izquierdo, float derecho);
    void setRangoDeteccion(float rango);

    TipoEnemigo getTipo() const;
    EstadoEnemigo getEstado() const;
    int getVida() const;
    int getDanio() const;
    bool estaVivo() const;
};

#endif // ENEMIGO_H