#ifndef NIVELRUNNER_H
#define NIVELRUNNER_H

#include "nivel.h"

class NivelRunner : public Nivel
{
private:
    float velocidadScroll;

    float tiempoTranscurrido;

    float duracionTormenta;

    bool tormentaActiva;

    float alturaMeta;

public:
    NivelRunner();

    void actualizar() override;

    void cargarNivel();

    void actualizarTiempo(float deltaTiempo);

    void activarTormenta();

    void desactivarTormenta();

    bool seAlcanzoLaMeta(float yJugador) const;

    float getVelocidadScroll() const;

    bool getTormentaActiva() const;

    float getAlturaMeta() const;
};

#endif // NIVELRUNNER_H
