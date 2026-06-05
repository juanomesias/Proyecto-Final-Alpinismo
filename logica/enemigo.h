#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "agenteinteligente.h"
#include "entidad.h"
#include "proyectil.h"
#include "../fisica/movimientoparabolico.h"

#include <memory>
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
    PERSIGUIENDO,
    PATRULLANDO,
    EMBISTIENDO,
    OSCILANDO,
    PRESIONANDO,
    INVOCANDO_APOYO,
    GIRANDO,
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
    bool invulnerable;
    float jugadorObjetivoX;
    float jugadorObjetivoY;
    float velocidadJugadorObjetivoX;
    float velocidadJugadorObjetivoY;
    bool jugadorEnAire;
    bool jugadorProtegiendo;
    bool jugadorAtacando;
    bool modoDuelo;
    std::unique_ptr<AgenteInteligente> ia;
    std::vector<Proyectil> proyectiles;
    MovimientoParabolico gravedadProyectiles;

    void actualizarPatrullero();
    void actualizarVolador();
    void actualizarJefe();
    void actualizarProyectiles();
    void lanzarProyectilDireccionado(float direccionX, float direccionY, float velocidadExtra = 0.0f);

public:
    Enemigo(float xInicial = 0.0f,
            float yInicial = 0.0f,
            TipoEnemigo tipo = PATRULLERO,
            int vida = 30,
            int danio = 10);

    Enemigo(Enemigo&&) noexcept = default;
    Enemigo& operator=(Enemigo&&) noexcept = default;
    Enemigo(const Enemigo&) = delete;
    Enemigo& operator=(const Enemigo&) = delete;

    void actualizar() override;
    void percibirJugador(float xJugador,
                         float yJugador,
                         float velocidadJugadorX = 0.0f,
                         float velocidadJugadorY = 0.0f,
                         bool jugadorEnAire = false,
                         bool jugadorProtegiendo = false,
                         bool jugadorAtacando = false,
                         bool modoDuelo = false);
    void recibirDanio(int cantidad);

    void setLimites(float izquierdo, float derecho);
    void setRangoDeteccion(float rango);
    void setVelocidades(float movimiento, float embiste);
    void consumirDecisionIA();

    TipoEnemigo getTipo() const;
    EstadoEnemigo getEstado() const;
    AtaqueIA getAtaqueIA() const;
    int getVida() const;
    int getDanio() const;
    bool estaVivo() const;
    bool estaInvulnerable() const;
    std::vector<Proyectil>& getProyectiles();
    const std::vector<Proyectil>& getProyectiles() const;
};

#endif // ENEMIGO_H
