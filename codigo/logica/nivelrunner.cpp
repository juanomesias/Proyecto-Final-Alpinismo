#include "nivelrunner.h"

NivelRunner::NivelRunner()
{
    velocidadScroll = 2.0f;
    tiempoTranscurrido = 0.0f;
    duracionTormenta = 20.0f;
    tormentaActiva = false;
    alturaMeta = 0.0f;

    cargarNivel();
}

void NivelRunner::actualizar()
{
    // - aumentar dificultad con el tiempo
    // - activar y desactivar tormenta
    // - ajustar scroll
}

void NivelRunner::cargarNivel()
{
    plataformas.clear();
    obstaculos.clear();

    agregarPlataforma(Plataforma(80, 520, 180, 30));
    agregarPlataforma(Plataforma(260, 450, 160, 30));
    agregarPlataforma(Plataforma(120, 370, 150, 30));
    agregarPlataforma(Plataforma(320, 300, 180, 30));
    agregarPlataforma(Plataforma(90, 220, 160, 30));
    agregarPlataforma(Plataforma(280, 140, 180, 30));

    agregarObstaculo(Obstaculo(200, 100, 20, 40, 40));
    agregarObstaculo(Obstaculo(340, 180, 20, 40, 40));
    agregarObstaculo(Obstaculo(140, 260, 25, 45, 45));
    agregarObstaculo(Obstaculo(260, 340, 25, 45, 45));

    // La meta está arriba de todo
    alturaMeta = 80.0f;
}

void NivelRunner::actualizarTiempo(float deltaTiempo)
{
    tiempoTranscurrido += deltaTiempo;

    if(tiempoTranscurrido >= duracionTormenta)
    {
        activarTormenta();
    }
}

void NivelRunner::activarTormenta()
{
    tormentaActiva = true;
    velocidadScroll = 3.5f;
}

void NivelRunner::desactivarTormenta()
{
    tormentaActiva = false;
    velocidadScroll = 2.0f;
}

bool NivelRunner::seAlcanzoLaMeta(float yJugador) const
{
    return yJugador <= alturaMeta;
}

float NivelRunner::getVelocidadScroll() const
{
    return velocidadScroll;
}

bool NivelRunner::getTormentaActiva() const
{
    return tormentaActiva;
}

float NivelRunner::getAlturaMeta() const
{
    return alturaMeta;
}