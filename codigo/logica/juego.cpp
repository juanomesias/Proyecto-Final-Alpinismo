#include "juego.h"

#include "nivelrunner.h"
#include "nivelplataforma.h"

Juego::Juego()
{
    nivelActual = nullptr;

    nivelActualID = 1;

    juegoTerminado = false;

    victoria = false;

    cargarNivel(1);
}

Juego::~Juego()
{
    delete nivelActual;
}

void Juego::actualizar()
{
    jugador.actualizar();

    if(nivelActual)
    {
        nivelActual->actualizar();
    }
}

void Juego::cargarNivel(int numeroNivel)
{
    delete nivelActual;

    nivelActual = nullptr;

    nivelActualID = numeroNivel;

    if(numeroNivel == 1)
    {
        nivelActual = new NivelRunner();
    }
    else if(numeroNivel == 2)
    {
        nivelActual = new NivelPlataforma();
    }
}

Jugador& Juego::getJugador()
{
    return jugador;
}

Nivel* Juego::getNivelActual() const
{
    return nivelActual;
}

bool Juego::estaTerminado() const
{
    return juegoTerminado;
}

bool Juego::hayVictoria() const
{
    return victoria;
}