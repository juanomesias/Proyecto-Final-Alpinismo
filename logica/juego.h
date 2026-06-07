#ifndef JUEGO_H
#define JUEGO_H

#include "jugador.h"
#include "nivel.h"

#include <memory>

class Juego
{
private:
    Jugador* jugador;
    std::unique_ptr<Nivel> nivel;
    int nivelActual;
    bool checkpointDueloFinalNivel2;
    float checkpointX;
    float checkpointY;

public:
    Juego();

    void setJugador(Jugador* nuevoJugador);
    Jugador* getJugador();
    const Jugador* getJugador() const;

    void establecerNivel(std::unique_ptr<Nivel> nuevoNivel, int idNivel);
    Nivel* getNivel();
    const Nivel* getNivel() const;

    int getNivelActual() const;
    void setNivelActual(int idNivel);

    void registrarCheckpointDueloFinalNivel2(float x, float y);
    void limpiarCheckpoint();
    bool tieneCheckpointDueloFinalNivel2() const;
    float getCheckpointX() const;
    float getCheckpointY() const;
    void restaurarJugadorEnCheckpoint();
};

#endif // JUEGO_H