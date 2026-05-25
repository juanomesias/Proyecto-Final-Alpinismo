#include "nivelplataforma.h"

NivelPlataforma::NivelPlataforma()
{
    posicionInicialJugadorX = 100;
    posicionInicialJugadorY = 100;

    metaX = 1400;
    metaY = 100;

    armaduraObtenida = false;

    cargarNivel();
}

void NivelPlataforma::actualizar()
{
    // Aquí luego puedes agregar lógica específica del nivel:
    // - revisar si el jugador ya alcanzó la meta
    // - mover plataformas si son móviles
    // - activar eventos del nivel
}

void NivelPlataforma::cargarNivel()
{
    plataformas.clear();
    obstaculos.clear();

    // Plataformas base del nivel
    agregarPlataforma(Plataforma(0, 550, 300, 30));
    agregarPlataforma(Plataforma(350, 480, 180, 30));
    agregarPlataforma(Plataforma(600, 420, 180, 30));
    agregarPlataforma(Plataforma(850, 360, 180, 30));
    agregarPlataforma(Plataforma(1100, 300, 200, 30));

    // Obstáculos simples del nivel
    agregarObstaculo(Obstaculo(420, 450, 20, 40, 40));
    agregarObstaculo(Obstaculo(760, 390, 20, 40, 40));
    agregarObstaculo(Obstaculo(1180, 260, 30, 50, 50));
}

void NivelPlataforma::obtenerArmadura()
{
    armaduraObtenida = true;
}

bool NivelPlataforma::seAlcanzoLaMeta(float xJugador, float yJugador) const
{
    return (xJugador >= metaX && yJugador <= metaY + 50);
}

float NivelPlataforma::getPosicionInicialJugadorX() const
{
    return posicionInicialJugadorX;
}

float NivelPlataforma::getPosicionInicialJugadorY() const
{
    return posicionInicialJugadorY;
}

float NivelPlataforma::getMetaX() const
{
    return metaX;
}

float NivelPlataforma::getMetaY() const
{
    return metaY;
}

bool NivelPlataforma::getArmaduraObtenida() const
{
    return armaduraObtenida;
}