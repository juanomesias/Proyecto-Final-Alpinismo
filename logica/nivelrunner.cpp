#include "nivelrunner.h"

#include <algorithm>

NivelRunner::NivelRunner()
{
    velocidadScroll = 3.0f;
    velocidadPersecucion = 2.15f;
    tiempoTranscurrido = 0.0f;
    duracionTormenta = 14.0f;
    tormentaActiva = false;
    metaX = 3200.0f;
    intervaloSpawnPatrulleros = 2.2f;
    intervaloSpawnVoladores = 4.3f;
    intervaloSpawnDisparadores = 6.2f;

    cargarNivel();
}

void NivelRunner::actualizar()
{
    if(tormentaActiva)
    {
        velocidadScroll = std::min(4.2f, velocidadScroll + 0.0018f);
        velocidadPersecucion = std::min(3.35f, velocidadPersecucion + 0.0015f);
    }
    else
    {
        velocidadScroll = std::min(3.4f, velocidadScroll + 0.0006f);
        velocidadPersecucion = std::min(2.55f, velocidadPersecucion + 0.0005f);
    }
}

void NivelRunner::cargarNivel()
{
    plataformas.clear();
    obstaculos.clear();

    agregarPlataforma(Plataforma(0, 520, 950, 80));
    agregarPlataforma(Plataforma(1080, 520, 520, 80));
    agregarPlataforma(Plataforma(1680, 520, 620, 80));
    agregarPlataforma(Plataforma(2380, 520, 980, 80));

    agregarPlataforma(Plataforma(620, 410, 160, 24));
    agregarPlataforma(Plataforma(1420, 360, 180, 24));
    agregarPlataforma(Plataforma(2140, 395, 170, 24));
    agregarPlataforma(Plataforma(2860, 340, 180, 24));

    agregarObstaculo(Obstaculo(420, 470, 20, 42, 42));
    agregarObstaculo(Obstaculo(1260, 470, 20, 42, 42));
    agregarObstaculo(Obstaculo(1980, 470, 25, 48, 48));
    agregarObstaculo(Obstaculo(2740, 470, 25, 48, 48));
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
    velocidadScroll = std::max(velocidadScroll, 3.8f);
    velocidadPersecucion = std::max(velocidadPersecucion, 2.9f);
}

void NivelRunner::desactivarTormenta()
{
    tormentaActiva = false;
    velocidadScroll = 3.2f;
    velocidadPersecucion = 2.3f;
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