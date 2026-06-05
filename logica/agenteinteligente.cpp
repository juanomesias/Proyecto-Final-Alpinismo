#include "agenteinteligente.h"

#include <algorithm>
#include <cmath>

AgenteInteligente::AgenteInteligente(int vidaMaxima)
{
    this->vidaMaxima = vidaMaxima;
    vidaActual = vidaMaxima;

    xEnemigo = 0.0f;
    yEnemigo = 0.0f;
    xJugador = 0.0f;
    yJugador = 0.0f;
    velocidadJugadorX = 0.0f;
    velocidadJugadorY = 0.0f;
    distanciaJugador = 0.0f;

    faseDos = false;
    invulnerable = false;
    jugadorEnAire = false;
    jugadorProtegiendo = false;
    jugadorAtacando = false;
    modoDuelo = false;

    tiempoDesdeUltimaDecision = 0.0f;
    tiempoEnAtaque = 0.0f;
    tiempoEnVulnerable = 0.0f;

    intervaloDecisionBase = 1.85f;
    intervaloDecisionActual = intervaloDecisionBase;
    duracionGiro = 1.65f;
    duracionVulnerable = 0.75f;
    rangoGiro = 150.0f;

    velocidadMovimientoBase = 2.9f;
    velocidadMovimientoActual = velocidadMovimientoBase;
    velocidadProyectilBase = 6.0f;
    velocidadProyectilActual = velocidadProyectilBase;
    direccionProyectilX = 1.0f;
    direccionProyectilY = 0.0f;

    preferenciaAerea = 0.0f;
    preferenciaDefensiva = 0.0f;
    preferenciaOfensiva = 0.0f;

    estadoActual = EstadoIA::OBSERVANDO;
    ataqueActual = AtaqueIA::ATAQUE_NINGUNO;
}

void AgenteInteligente::percibir(float xEnemigo,
                                 float yEnemigo,
                                 float xJugador,
                                 float yJugador,
                                 float velocidadJugadorX,
                                 float velocidadJugadorY,
                                 bool jugadorEnAire,
                                 bool jugadorProtegiendo,
                                 bool jugadorAtacando,
                                 bool modoDuelo,
                                 int vidaActual)
{
    this->xEnemigo = xEnemigo;
    this->yEnemigo = yEnemigo;
    this->xJugador = xJugador;
    this->yJugador = yJugador;
    this->velocidadJugadorX = velocidadJugadorX;
    this->velocidadJugadorY = velocidadJugadorY;
    this->jugadorEnAire = jugadorEnAire;
    this->jugadorProtegiendo = jugadorProtegiendo;
    this->jugadorAtacando = jugadorAtacando;
    this->modoDuelo = modoDuelo;
    this->vidaActual = vidaActual;

    calcularDistancia();
}

void AgenteInteligente::calcularDistancia()
{
    const float dx = xJugador - xEnemigo;
    const float dy = yJugador - yEnemigo;
    distanciaJugador = std::sqrt(dx * dx + dy * dy);
}

void AgenteInteligente::calcularDireccionPredictiva(float tiempoProyeccion)
{
    const float objetivoX = xJugador + velocidadJugadorX * tiempoProyeccion;
    const float objetivoY = yJugador + velocidadJugadorY * tiempoProyeccion;

    float dx = objetivoX - xEnemigo;
    float dy = objetivoY - yEnemigo;
    float magnitud = std::sqrt(dx * dx + dy * dy);

    if(magnitud <= 0.0f)
    {
        dx = 1.0f;
        dy = 0.0f;
        magnitud = 1.0f;
    }

    direccionProyectilX = dx / magnitud;
    direccionProyectilY = dy / magnitud;
}

void AgenteInteligente::razonar()
{
    ataqueActual = AtaqueIA::ATAQUE_NINGUNO;

    if(!faseDos && vidaActual <= vidaMaxima / 2)
        activarFaseDos();

    if(estadoActual == EstadoIA::GOLPE_GIRATORIO ||
        estadoActual == EstadoIA::RECUPERANDO_IA)
        return;

    if(tiempoDesdeUltimaDecision < intervaloDecisionActual)
    {
        estadoActual = modoDuelo ? EstadoIA::PERSIGUIENDO : EstadoIA::OBSERVANDO;
        return;
    }

    if(modoDuelo)
    {
        if(distanciaJugador <= rangoGiro)
        {
            ataqueActual = AtaqueIA::ATAQUE_GIRO;
            estadoActual = EstadoIA::GOLPE_GIRATORIO;
            invulnerable = true;
            tiempoEnAtaque = 0.0f;
            tiempoDesdeUltimaDecision = 0.0f;
            return;
        }

        if(jugadorEnAire || preferenciaAerea >= 0.40f)
        {
            ataqueActual = AtaqueIA::ATAQUE_OSCILATORIO;
            estadoActual = EstadoIA::OSCILANDO;
            tiempoDesdeUltimaDecision = 0.0f;
            calcularDireccionPredictiva(0.35f);
            return;
        }

        ataqueActual = AtaqueIA::ATAQUE_PROYECTIL_PREDICTIVO;
        estadoActual = EstadoIA::DISPARANDO;
        tiempoDesdeUltimaDecision = 0.0f;
        calcularDireccionPredictiva(0.45f);
        return;
    }

    if(preferenciaDefensiva >= 0.30f || distanciaJugador < 210.0f)
    {
        ataqueActual = AtaqueIA::ATAQUE_ACELERAR_CAZA;
        estadoActual = EstadoIA::ACELERANDO_CAZA;
        tiempoDesdeUltimaDecision = 0.0f;
        return;
    }

    if(jugadorEnAire || preferenciaAerea >= 0.48f)
    {
        ataqueActual = AtaqueIA::ATAQUE_OSCILATORIO;
        estadoActual = EstadoIA::OSCILANDO;
        tiempoDesdeUltimaDecision = 0.0f;
        calcularDireccionPredictiva(0.55f);
        return;
    }

    if(preferenciaOfensiva >= 0.28f)
    {
        ataqueActual = AtaqueIA::ATAQUE_INVOCAR_APOYO;
        estadoActual = EstadoIA::INVOCANDO_APOYO;
        tiempoDesdeUltimaDecision = 0.0f;
        return;
    }

    ataqueActual = AtaqueIA::ATAQUE_PROYECTIL_PREDICTIVO;
    estadoActual = EstadoIA::DISPARANDO;
    tiempoDesdeUltimaDecision = 0.0f;
    calcularDireccionPredictiva(0.65f);
}

void AgenteInteligente::actuar(float deltaTiempo)
{
    tiempoDesdeUltimaDecision += deltaTiempo;

    if(faseDos)
    {
        velocidadMovimientoActual = velocidadMovimientoBase * 1.35f;
        velocidadProyectilActual = velocidadProyectilBase * 1.25f;
        intervaloDecisionActual = 1.05f;
    }
    else
    {
        velocidadMovimientoActual = velocidadMovimientoBase;
        velocidadProyectilActual = velocidadProyectilBase;
        intervaloDecisionActual = intervaloDecisionBase;
    }

    if(preferenciaAerea >= 0.55f)
        velocidadProyectilActual = std::max(velocidadProyectilActual, velocidadProyectilBase * 1.15f);

    if(preferenciaDefensiva >= 0.45f)
        intervaloDecisionActual = std::min(intervaloDecisionActual, 0.95f);

    if(estadoActual == EstadoIA::GOLPE_GIRATORIO)
    {
        tiempoEnAtaque += deltaTiempo;
        if(tiempoEnAtaque >= duracionGiro)
        {
            estadoActual = EstadoIA::RECUPERANDO_IA;
            invulnerable = false;
            tiempoEnAtaque = 0.0f;
            tiempoEnVulnerable = 0.0f;
        }
    }
    else if(estadoActual == EstadoIA::RECUPERANDO_IA)
    {
        tiempoEnVulnerable += deltaTiempo;
        if(tiempoEnVulnerable >= duracionVulnerable)
        {
            estadoActual = EstadoIA::OBSERVANDO;
            tiempoEnVulnerable = 0.0f;
        }
    }
}

void AgenteInteligente::actualizarPreferencias()
{
    auto promedio = [](const std::vector<float>& historial) -> float
    {
        if(historial.empty())
            return 0.0f;

        float suma = 0.0f;
        for(float valor : historial)
            suma += valor;

        return suma / static_cast<float>(historial.size());
    };

    preferenciaAerea = promedio(historialSaltos);
    preferenciaDefensiva = promedio(historialDefensa);
    preferenciaOfensiva = promedio(historialAtaque);
}

void AgenteInteligente::aprender()
{
    historialDistancias.push_back(distanciaJugador);
    historialSaltos.push_back(jugadorEnAire ? 1.0f : 0.0f);
    historialDefensa.push_back(jugadorProtegiendo ? 1.0f : 0.0f);
    historialAtaque.push_back(jugadorAtacando ? 1.0f : 0.0f);

    auto recortarHistorial = [](std::vector<float>& historial)
    {
        if(historial.size() > 36)
            historial.erase(historial.begin());
    };

    recortarHistorial(historialDistancias);
    recortarHistorial(historialSaltos);
    recortarHistorial(historialDefensa);
    recortarHistorial(historialAtaque);

    actualizarPreferencias();

    if(historialDistancias.size() < 10)
        return;

    float sumaDistancias = 0.0f;
    for(float distancia : historialDistancias)
        sumaDistancias += distancia;

    const float promedioDistancia = sumaDistancias /
                                    static_cast<float>(historialDistancias.size());

    if(promedioDistancia > 260.0f)
        velocidadProyectilActual = std::max(velocidadProyectilActual,
                                            velocidadProyectilBase * 1.25f);
    else if(promedioDistancia < 150.0f)
        velocidadMovimientoActual = std::max(velocidadMovimientoActual,
                                             velocidadMovimientoBase * 1.25f);
}

void AgenteInteligente::activarFaseDos()
{
    faseDos = true;
    intervaloDecisionActual = 1.0f;
    velocidadMovimientoActual = velocidadMovimientoBase * 1.35f;
    velocidadProyectilActual = velocidadProyectilBase * 1.25f;
}

void AgenteInteligente::consumirDecision()
{
    if(estadoActual == EstadoIA::DISPARANDO ||
        estadoActual == EstadoIA::OSCILANDO ||
        estadoActual == EstadoIA::ACELERANDO_CAZA ||
        estadoActual == EstadoIA::INVOCANDO_APOYO)
    {
        estadoActual = EstadoIA::OBSERVANDO;
    }

    ataqueActual = AtaqueIA::ATAQUE_NINGUNO;
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

float AgenteInteligente::getIntervaloDecision() const
{
    return intervaloDecisionActual;
}

float AgenteInteligente::getDuracionGiro() const
{
    return duracionGiro;
}

float AgenteInteligente::getDuracionVulnerable() const
{
    return duracionVulnerable;
}

float AgenteInteligente::getPreferenciaAerea() const
{
    return preferenciaAerea;
}

float AgenteInteligente::getPreferenciaDefensiva() const
{
    return preferenciaDefensiva;
}

float AgenteInteligente::getPreferenciaOfensiva() const
{
    return preferenciaOfensiva;
}