#include "jugador.h"

Jugador::Jugador(float xInicial, float yInicial)
    : Entidad(xInicial, yInicial)
{
    vida = 100;
    puntaje = 0;

    nivel = 1;
    experiencia = 0;

    tieneArmadura = false;
    enSuelo = false;
    velocidadExtra = false;

    fuerzaSalto = 10;

    velocidadNormal = 5;
    velocidadActual = 5;
}

void Jugador::actualizar()
{
    mover();
}

void Jugador::saltar()
{
    if(enSuelo)
    {
        velocidadY = -fuerzaSalto;
        enSuelo = false;
    }
}

void Jugador::recibirDanio(int cantidad)
{
    if(tieneArmadura)
    {
        cantidad /= 2;
    }

    vida -= cantidad;

    if(vida < 0)
    {
        vida = 0;
    }
}

void Jugador::obtenerArmadura()
{
    tieneArmadura = true;
    fuerzaSalto = 12;
}

void Jugador::ganarExperiencia(int cantidad)
{
    experiencia += cantidad;

    if(experiencia >= 100)
    {
        experiencia = 0;
        nivel++;
    }
}

void Jugador::activarVelocidad()
{
    velocidadExtra = true;
    velocidadActual = 8;
}

void Jugador::desactivarVelocidad()
{
    velocidadExtra = false;
    velocidadActual = velocidadNormal;
}

int Jugador::getVida() const
{
    return vida;
}

bool Jugador::getTieneArmadura() const
{
    return tieneArmadura;
}

int Jugador::getPuntaje() const
{
    return puntaje;
}

int Jugador::getNivel() const
{
    return nivel;
}

int Jugador::getExperiencia() const
{
    return experiencia;
}

float Jugador::getFuerzaSalto() const
{
    return fuerzaSalto;
}

bool Jugador::getEnSuelo() const
{
    return enSuelo;
}

bool Jugador::getVelocidadExtra() const
{
    return velocidadExtra;
}

float Jugador::getVelocidadActual() const
{
    return velocidadActual;
}

void Jugador::setEnSuelo(bool estado)
{
    enSuelo = estado;
}

void Jugador::setVida(int nuevaVida)
{
    vida = nuevaVida;
}

void Jugador::setPuntaje(int nuevoPuntaje)
{
    puntaje = nuevoPuntaje;
}