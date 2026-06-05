#ifndef AGENTEINTELIGENTE_H
#define AGENTEINTELIGENTE_H

#include <vector>

enum class EstadoIA
{
    OBSERVANDO,
    PERSIGUIENDO,
    DISPARANDO,
    OSCILANDO,
    ACELERANDO_CAZA,
    INVOCANDO_APOYO,
    GOLPE_GIRATORIO,
    RECUPERANDO_IA
};

enum class AtaqueIA
{
    ATAQUE_NINGUNO,
    ATAQUE_PROYECTIL_PREDICTIVO,
    ATAQUE_OSCILATORIO,
    ATAQUE_ACELERAR_CAZA,
    ATAQUE_INVOCAR_APOYO,
    ATAQUE_GIRO,
};

class AgenteInteligente
{
private:
    float xEnemigo;
    float yEnemigo;
    float xJugador;
    float yJugador;
    float velocidadJugadorX;
    float velocidadJugadorY;
    float distanciaJugador;

    int vidaMaxima;
    int vidaActual;

    bool faseDos;
    bool invulnerable;
    bool jugadorEnAire;
    bool jugadorProtegiendo;
    bool jugadorAtacando;
    bool modoDuelo;

    float tiempoDesdeUltimaDecision;
    float tiempoEnAtaque;
    float tiempoEnVulnerable;

    float intervaloDecisionBase;
    float intervaloDecisionActual;
    float duracionGiro;
    float duracionVulnerable;
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
    std::vector<float> historialSaltos;
    std::vector<float> historialDefensa;
    std::vector<float> historialAtaque;
    float preferenciaAerea;
    float preferenciaDefensiva;
    float preferenciaOfensiva;

    void calcularDistancia();
    void activarFaseDos();
    void calcularDireccionPredictiva(float tiempoProyeccion);
    void actualizarPreferencias();

public:
    AgenteInteligente(int vidaMaxima = 100);

    void percibir(float xEnemigo,
                  float yEnemigo,
                  float xJugador,
                  float yJugador,
                  float velocidadJugadorX,
                  float velocidadJugadorY,
                  bool jugadorEnAire,
                  bool jugadorProtegiendo,
                  bool jugadorAtacando,
                  bool modoDuelo,
                  int vidaActual);
    void razonar();
    void actuar(float deltaTiempo);
    void aprender();
    void consumirDecision();

    EstadoIA getEstadoActual() const;
    AtaqueIA getAtaqueActual() const;

    bool estaEnFaseDos() const;
    bool estaInvulnerable() const;

    float getDistanciaJugador() const;
    float getVelocidadMovimientoActual() const;
    float getVelocidadProyectilActual() const;
    float getDireccionProyectilX() const;
    float getDireccionProyectilY() const;
    float getIntervaloDecision() const;
    float getDuracionGiro() const;
    float getDuracionVulnerable() const;
    float getPreferenciaAerea() const;
    float getPreferenciaDefensiva() const;
    float getPreferenciaOfensiva() const;
};

#endif // AGENTEINTELIGENTE_H
