#include "nivelrunner.h"

#include <algorithm>

NivelRunner::NivelRunner()
{
    velocidadScroll = 3.2f;
    velocidadPersecucion = 2.2f;
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
        velocidadScroll = std::min(4.4f, velocidadScroll + 0.0015f);
        velocidadPersecucion = std::min(3.6f, velocidadPersecucion + 0.0014f);
    }
    else
    {
        velocidadScroll = std::min(3.7f, velocidadScroll + 0.0005f);
        velocidadPersecucion = std::min(2.8f, velocidadPersecucion + 0.0004f);
    }
}

void NivelRunner::cargarNivel()
{
    plataformas.clear();
    obstaculos.clear();

    float xBase = 0.0f;
    const float anchoSegmento = 820.0f;
    const float separacion = 120.0f;

    for(int i = 0; i < 14; ++i)
    {
        agregarPlataforma(Plataforma(xBase, 520, anchoSegmento, 80));

        if(i % 2 == 0)
            agregarPlataforma(Plataforma(xBase + 250.0f, 390.0f - (i % 3) * 25.0f, 190, 26));
        else
            agregarPlataforma(Plataforma(xBase + 410.0f, 345.0f + (i % 3) * 20.0f, 210, 26));

        if(i % 3 == 1)
            agregarPlataforma(Plataforma(xBase + 110.0f, 300.0f, 150, 24));

        agregarObstaculo(Obstaculo(xBase + 300.0f, 470.0f, 18, 40, 40));
        if(i % 2 == 1)
            agregarObstaculo(Obstaculo(xBase + 620.0f, 470.0f, 22, 46, 46));

        xBase += anchoSegmento + separacion;
    }

    agregarPlataforma(Plataforma(inicioDueloFinal, 520, finDueloFinal - inicioDueloFinal + 300.0f, 80));
    agregarPlataforma(Plataforma(inicioDueloFinal + 260.0f, 360.0f, 230.0f, 24.0f));
    agregarPlataforma(Plataforma(inicioDueloFinal + 860.0f, 330.0f, 220.0f, 24.0f));
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
    velocidadScroll = std::max(velocidadScroll, 3.95f);
    velocidadPersecucion = std::max(velocidadPersecucion, 3.05f);
}

void NivelRunner::desactivarTormenta()
{
    tormentaActiva = false;
    velocidadScroll = 3.35f;
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