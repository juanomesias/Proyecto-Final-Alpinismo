#include "nivelrunner.h"

#include <algorithm>

NivelRunner::NivelRunner()
{
    velocidadScroll = 2.8f;
    velocidadPersecucion = 2.25f;
    tiempoTranscurrido = 0.0f;
    duracionTormenta = 18.0f;
    tormentaActiva = false;
    inicioDueloFinal = 11050.0f;
    finDueloFinal = 12550.0f;
    metaX = inicioDueloFinal;
    intervaloSpawnPatrulleros = 7.0f;
    intervaloSpawnVoladores = 9.0f;
    intervaloSpawnDisparadores = 12.0f;

    cargarNivel();
}

void NivelRunner::actualizar()
{
    if(tormentaActiva)
    {
        velocidadScroll = std::min(3.85f, velocidadScroll + 0.0012f);
        velocidadPersecucion = std::min(3.35f, velocidadPersecucion + 0.00135f);
    }
    else
    {
        velocidadScroll = std::min(3.2f, velocidadScroll + 0.00035f);
        velocidadPersecucion = std::min(2.75f, velocidadPersecucion + 0.00045f);
    }
}

void NivelRunner::cargarNivel()
{
    plataformas.clear();
    obstaculos.clear();

    float xBase = 0.0f;
    const float anchoSegmento = 820.0f;
    const float separacion = 120.0f;
    const float alturas[3] = {430.0f, 370.0f, 315.0f};

    for(int i = 0; i < 14; ++i)
    {
        agregarPlataforma(Plataforma(xBase, 520, anchoSegmento, 80));

        agregarPlataforma(Plataforma(xBase + 135.0f, alturas[i % 3], 205, 26));
        agregarPlataforma(Plataforma(xBase + 430.0f, alturas[(i + 1) % 3], 220, 26));

        if(i % 2 == 0)
            agregarPlataforma(Plataforma(xBase + 650.0f, alturas[(i + 2) % 3], 165, 24));

        agregarObstaculo(Obstaculo(xBase + 300.0f, 470.0f, 18, 40, 40));
        if(i % 2 == 1)
            agregarObstaculo(Obstaculo(xBase + 620.0f, 470.0f, 22, 46, 46));
        if(i % 3 == 0)
            agregarObstaculo(Obstaculo(xBase + 205.0f, alturas[i % 3] - 38.0f, 14, 34, 38));
        if(i % 4 == 2)
            agregarObstaculo(Obstaculo(xBase + 505.0f, alturas[(i + 1) % 3] - 34.0f, 16, 32, 34));

        xBase += anchoSegmento + separacion;
    }

    agregarPlataforma(Plataforma(inicioDueloFinal, 520, finDueloFinal - inicioDueloFinal + 300.0f, 80));
    agregarPlataforma(Plataforma(inicioDueloFinal + 190.0f, 420.0f, 230.0f, 24.0f));
    agregarPlataforma(Plataforma(inicioDueloFinal + 560.0f, 360.0f, 230.0f, 24.0f));
    agregarPlataforma(Plataforma(inicioDueloFinal + 900.0f, 315.0f, 220.0f, 24.0f));
}

void NivelRunner::actualizarTiempo(float deltaTiempo)
{
    tiempoTranscurrido += deltaTiempo;

    if(!tormentaActiva && tiempoTranscurrido >= duracionTormenta)
        activarTormenta();
}

void NivelRunner::activarTormenta()
{
    tormentaActiva = true;
    velocidadScroll = std::max(velocidadScroll, 3.35f);
    velocidadPersecucion = std::max(velocidadPersecucion, 3.0f);
}

void NivelRunner::desactivarTormenta()
{
    tormentaActiva = false;
    velocidadScroll = 2.95f;
    velocidadPersecucion = 2.35f;
}

bool NivelRunner::seAlcanzoLaMeta(float xJugador) const
{
    return xJugador >= metaX;
}

float NivelRunner::getVelocidadScroll() const
{
    return velocidadScroll;
}

float NivelRunner::getVelocidadPersecucion() const
{
    return velocidadPersecucion;
}

bool NivelRunner::getTormentaActiva() const
{
    return tormentaActiva;
}

float NivelRunner::getMetaX() const
{
    return metaX;
}

float NivelRunner::getInicioDueloFinal() const
{
    return inicioDueloFinal;
}

float NivelRunner::getFinDueloFinal() const
{
    return finDueloFinal;
}

float NivelRunner::getIntervaloSpawnPatrulleros() const
{
    return intervaloSpawnPatrulleros;
}

float NivelRunner::getIntervaloSpawnVoladores() const
{
    return intervaloSpawnVoladores;
}

float NivelRunner::getIntervaloSpawnDisparadores() const
{
    return intervaloSpawnDisparadores;
}