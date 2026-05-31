#ifndef JUEGO_H
#define JUEGO_H

#include "jugador.h"
#include "nivel.h"

class Juego
{
private:
    Jugador jugador;

    Nivel* nivelActual;

    int nivelActualID;

    bool juegoTerminado;

    bool victoria;

public:
    Juego();

    ~Juego();

    void actualizar();

    void cargarNivel(int numeroNivel);

    Jugador& getJugador();

    Nivel* getNivelActual() const;

    bool estaTerminado() const;

    bool hayVictoria() const;
};

#endif // JUEGO_H