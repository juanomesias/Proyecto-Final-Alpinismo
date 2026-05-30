#ifndef AGENTEINTELIGENTE_H
#define AGENTEINTELIGENTE_H

#include <vector>

enum EstadoIA
{
    OBSERVANDO,
    DISPARANDO,
    GOLPE_GIRATORIO,
    RECUPERANDO,
    SALVA_RADIAL
};

enum AtaqueIA
{
    ATAQUE_NINGUNO,
    ATAQUE_PROYECTIL,
    ATAQUE_GIRO,
    ATAQUE_SALVA_RADIAL
};

class AgenteInteligente
{
private:
    float xEnemigo;
    float yEnemigo;

    float xJugador;
    float yJugador;

    float distanciaJugador;

    int vidaMaxima;
    int vidaActual;

    bool faseDos;
    bool invulnerable;
    bool salvaEjecutada;

    bool disparoPendiente;
    bool giroPendiente;
    bool salvaPendiente;

    float tiempoDesdeUltimoDisparo;
    float tiempoEnAtaque;
    float tiempoEnVulnerable;

    float intervaloDisparo;
    float duracionGiro;
    float duracionVulnerable;
    float duracionSalva;

    float rangoGiro;

    float velocidadMovimientoBase;
    float velocidadMovimientoActual;

    float velocidadProyectilBase;
    float velocidadProyectilActual;

    float direccionProyectilX;
    float direccionProyectilY;

    EstadoIA estadoActual;
    AtaqueIA ataqueActual;

    std::vector<float> historialDistancias;

    void calcularDistancia();
    void activarFaseDos();
    void ajustarFaseDos();

public:
    AgenteInteligente(int vidaMaxima = 100);

    void percibir(float xEnemigo, float yEnemigo,
                  float xJugador, float yJugador,
                  int vidaActual);

    void razonar();

    void actuar(float deltaTiempo);

    void aprender();

    void consumirDecision();

    EstadoIA getEstadoActual() const;
    AtaqueIA getAtaqueActual() const;

    bool estaEnFaseDos() const;
    bool estaInvulnerable() const;
    bool fueSalvaEjecutada() const;

    float getDistanciaJugador() const;

    float getVelocidadMovimientoActual() const;
    float getVelocidadProyectilActual() const;

    float getDireccionProyectilX() const;
    float getDireccionProyectilY() const;

    float getIntervaloDisparo() const;
    float getDuracionGiro() const;
    float getDuracionVulnerable() const;
};

#endif // AGENTEINTELIGENTE_H