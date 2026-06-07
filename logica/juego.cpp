#include "juego.h"

#include <utility>

Juego::Juego()
{
    jugador = nullptr;
    nivelActual = 1;
    checkpointDueloFinalNivel2 = false;
    checkpointX = 0.0f;
    checkpointY = 0.0f;
}

void Juego::setJugador(Jugador* nuevoJugador)
{
    jugador = nuevoJugador;
}

Jugador* Juego::getJugador()
{
    return jugador;
}

const Jugador* Juego::getJugador() const
{
    return jugador;
}

void Juego::establecerNivel(std::unique_ptr<Nivel> nuevoNivel, int idNivel)
{
    nivel = std::move(nuevoNivel);
    nivelActual = idNivel;
}

Nivel* Juego::getNivel()
{
    return nivel.get();
}

const Nivel* Juego::getNivel() const
{
    return nivel.get();
}

int Juego::getNivelActual() const
{
    return nivelActual;
}

void Juego::setNivelActual(int idNivel)
{
    nivelActual = idNivel;
}

void Juego::registrarCheckpointDueloFinalNivel2(float x, float y)
{
    checkpointDueloFinalNivel2 = true;
    checkpointX = x;
    checkpointY = y;
}

void Juego::limpiarCheckpoint()
{
    checkpointDueloFinalNivel2 = false;
    checkpointX = 0.0f;
    checkpointY = 0.0f;
}

bool Juego::tieneCheckpointDueloFinalNivel2() const
{
    return checkpointDueloFinalNivel2;
}

float Juego::getCheckpointX() const
{
    return checkpointX;
}

float Juego::getCheckpointY() const
{
    return checkpointY;
}

void Juego::restaurarJugadorEnCheckpoint()
{
    if(!jugador || !checkpointDueloFinalNivel2)
        return;

    jugador->setX(checkpointX);
    jugador->setY(checkpointY);
    jugador->setVelocidadX(0.0f);
    jugador->setVelocidadY(0.0f);
    jugador->setEnSuelo(true);
}
