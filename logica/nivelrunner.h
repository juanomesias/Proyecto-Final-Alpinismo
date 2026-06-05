#ifndef NIVELRUNNER_H
#define NIVELRUNNER_H

#include "nivel.h"

class NivelRunner : public Nivel
{
private:
    float velocidadScroll;
    float velocidadPersecucion;
    float tiempoTranscurrido;
    float duracionTormenta;
    bool tormentaActiva;
    float metaX;
    float intervaloSpawnPatrulleros;
    float intervaloSpawnVoladores;
    float intervaloSpawnDisparadores;

public:
    NivelRunner();

    void actualizar() override;
    void cargarNivel();
    void actualizarTiempo(float deltaTiempo);
    void activarTormenta();
    void desactivarTormenta();

    bool seAlcanzoLaMeta(float xJugador) const;

    float getVelocidadScroll() const;
    float getVelocidadPersecucion() const;
    bool getTormentaActiva() const;
    float getMetaX() const;
    float getIntervaloSpawnPatrulleros() const;
    float getIntervaloSpawnVoladores() const;
    float getIntervaloSpawnDisparadores() const;
};

#endif // NIVELRUNNER_H