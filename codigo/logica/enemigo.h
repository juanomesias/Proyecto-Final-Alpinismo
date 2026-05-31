#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "entidad.h"
#include "proyectil.h"
#include "agenteinteligente.h"
#include "movimientoparabolico.h"
#include <vector>

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
    GIRANDO,
    RECUPERANDO,
    ATAQUE_RADIAL
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

    bool invulnerable;
    bool salvaRadialGenerada;

    float jugadorObjetivoX;
    float jugadorObjetivoY;

    AgenteInteligente* ia;
    std::vector<Proyectil> proyectiles;
    MovimientoParabolico gravedadProyectiles;

    void actualizarPatrullero();
    void actualizarVolador();
    void actualizarJefe();
    void actualizarProyectiles();

    void lanzarProyectilDireccionado(float direccionX,float direccionY);
    void lanzarSalvaRadial();

public:
    Enemigo(float xInicial = 0,
            float yInicial = 0,
            TipoEnemigo tipo = PATRULLERO,
            int vida = 30,
            int danio = 10);

    void actualizar() override;

    void percibirJugador(float xJugador, float yJugador);

    void recibirDanio(int cantidad);

    void setLimites(float izquierdo, float derecho);
    void setRangoDeteccion(float rango);

    TipoEnemigo getTipo() const;
    EstadoEnemigo getEstado() const;
    int getVida() const;
    int getDanio() const;
    bool estaVivo() const;
    bool estaInvulnerable() const;

    const std::vector<Proyectil>& getProyectiles() const;
    ~Enemigo() override;
};

#endif // ENEMIGO_H