#include "agenteinteligente.h"

#include <cmath>

AgenteInteligente::AgenteInteligente(int vidaMaxima)
{
    this->vidaMaxima = vidaMaxima;
    vidaActual = vidaMaxima;

    xEnemigo = 0.0f;
    yEnemigo = 0.0f;
    xJugador = 0.0f;
    yJugador = 0.0f;
    distanciaJugador = 0.0f;

    faseDos = false;
    invulnerable = false;
    salvaEjecutada = false;

    tiempoDesdeUltimoDisparo = 0.0f;
    tiempoEnAtaque = 0.0f;
    tiempoEnVulnerable = 0.0f;

    intervaloDisparo = 1.8f;
    duracionGiro = 2.2f;
    duracionVulnerable = 0.9f;
    duracionSalva = 1.3f;
    rangoGiro = 180.0f;

    velocidadMovimientoBase = 2.8f;
    velocidadMovimientoActual = velocidadMovimientoBase;
    velocidadProyectilBase = 6.0f;
    velocidadProyectilActual = velocidadProyectilBase;
    direccionProyectilX = 1.0f;
    direccionProyectilY = 0.0f;

    estadoActual = OBSERVANDO;
    ataqueActual = ATAQUE_NINGUNO;
}

void AgenteInteligente::percibir(float xEnemigo,
                                 float yEnemigo,
                                 float xJugador,
                                 float yJugador,
                                 int vidaActual)
{
    this->xEnemigo = xEnemigo;
    this->yEnemigo = yEnemigo;
    this->xJugador = xJugador;
    this->yJugador = yJugador;
    this->vidaActual = vidaActual;

    calcularDistancia();
}

void AgenteInteligente::calcularDistancia()
{
    const float dx = xJugador - xEnemigo;
    const float dy = yJugador - yEnemigo;
    distanciaJugador = std::sqrt(dx * dx + dy * dy);
}

void AgenteInteligente::razonar()
{
    ataqueActual = ATAQUE_NINGUNO;

    if(!faseDos && vidaActual <= vidaMaxima / 2)
    {
        activarFaseDos();
        ataqueActual = ATAQUE_SALVA_RADIAL;
        estadoActual = SALVA_RADIAL;
        tiempoEnAtaque = 0.0f;
        return;
    }

    if(estadoActual == GOLPE_GIRATORIO ||
        estadoActual == RECUPERANDO_IA ||
        estadoActual == SALVA_RADIAL)
    {
        return;
    }

    if(tiempoDesdeUltimoDisparo < intervaloDisparo)
        return;

    if(distanciaJugador <= rangoGiro)
    {
        ataqueActual = ATAQUE_GIRO;
        estadoActual = GOLPE_GIRATORIO;
        invulnerable = true;
        tiempoEnAtaque = 0.0f;
        tiempoDesdeUltimoDisparo = 0.0f;
        return;
    }

    ataqueActual = ATAQUE_PROYECTIL;
    estadoActual = DISPARANDO;
    tiempoDesdeUltimoDisparo = 0.0f;

    if(distanciaJugador > 0.0f)
    {
        const float dx = xJugador - xEnemigo;
        const float dy = yJugador - yEnemigo;
        direccionProyectilX = dx / distanciaJugador;
        direccionProyectilY = dy / distanciaJugador;
    }
}

void AgenteInteligente::actuar(float deltaTiempo)
{
    tiempoDesdeUltimoDisparo += deltaTiempo;

    if(faseDos)
    {
        velocidadMovimientoActual = velocidadMovimientoBase * 1.45f;
        velocidadProyectilActual = velocidadProyectilBase * 1.2f;
        intervaloDisparo = 1.0f;
    }

    if(estadoActual == GOLPE_GIRATORIO)
    {
        tiempoEnAtaque += deltaTiempo;
        if(tiempoEnAtaque >= duracionGiro)
        {
            estadoActual = RECUPERANDO_IA;
            invulnerable = false;
            tiempoEnAtaque = 0.0f;
            tiempoEnVulnerable = 0.0f;
        }
    }
    else if(estadoActual == RECUPERANDO_IA)
    {
        tiempoEnVulnerable += deltaTiempo;
        if(tiempoEnVulnerable >= duracionVulnerable)
        {
            estadoActual = OBSERVANDO;
            tiempoEnVulnerable = 0.0f;
        }
    }
    else if(estadoActual == SALVA_RADIAL)
    {
        tiempoEnAtaque += deltaTiempo;
        if(tiempoEnAtaque >= duracionSalva)
        {
            estadoActual = OBSERVANDO;
            tiempoEnAtaque = 0.0f;
            salvaEjecutada = true;
        }
    }
}

void AgenteInteligente::aprender()
{
    historialDistancias.push_back(distanciaJugador);
    if(historialDistancias.size() > 18)
        historialDistancias.erase(historialDistancias.begin());

    if(historialDistancias.size() < 8)
        return;

    float suma = 0.0f;
    for(float distancia : historialDistancias)
        suma += distancia;

    const float promedio = suma / static_cast<float>(historialDistancias.size());
    if(promedio > 230.0f)
    {
        intervaloDisparo = std::min(intervaloDisparo, 0.9f);
        velocidadProyectilActual = velocidadProyectilBase * 1.35f;
    }
    else if(promedio < 120.0f)
    {
        velocidadMovimientoActual = velocidadMovimientoBase * 1.55f;
    }
}

void AgenteInteligente::activarFaseDos()
{
    faseDos = true;
    velocidadMovimientoActual = velocidadMovimientoBase * 1.45f;
    velocidadProyectilActual = velocidadProyectilBase * 1.2f;
    intervaloDisparo = 1.0f;
}

void AgenteInteligente::consumirDecision()
{
    if(estadoActual == DISPARANDO)
        estadoActual = OBSERVANDO;

    ataqueActual = ATAQUE_NINGUNO;
}

EstadoIA AgenteInteligente::getEstadoActual() const
{
    return estadoActual;
}

AtaqueIA AgenteInteligente::getAtaqueActual() const
{
    return ataqueActual;
}

bool AgenteInteligente::estaEnFaseDos() const
{
    return faseDos;
}

bool AgenteInteligente::estaInvulnerable() const
{
    return invulnerable;
}

bool AgenteInteligente::fueSalvaEjecutada() const
{
    return salvaEjecutada;
}

float AgenteInteligente::getDistanciaJugador() const
{
    return distanciaJugador;
}

float AgenteInteligente::getVelocidadMovimientoActual() const
{
    return velocidadMovimientoActual;
}

float AgenteInteligente::getVelocidadProyectilActual() const
{
    return velocidadProyectilActual;
}

float AgenteInteligente::getDireccionProyectilX() const
{
    return direccionProyectilX;
}

float AgenteInteligente::getDireccionProyectilY() const
{
    return direccionProyectilY;
}

float AgenteInteligente::getIntervaloDisparo() const
{
    return intervaloDisparo;
}

float AgenteInteligente::getDuracionGiro() const
{
    return duracionGiro;
}

float AgenteInteligente::getDuracionVulnerable() const
{
    return duracionVulnerable;
}