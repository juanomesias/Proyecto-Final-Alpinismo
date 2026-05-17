#include "jugador.h"

Jugador::Jugador(float xInicial, float yInicial)
    : Entidad(xInicial, yInicial)
{
    vida = 100;
    tieneArmadura = false;
    puntaje = 0;
    fuerzaSalto = 18;
    enSuelo = false;
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
    if(tieneArmadura){
        cantidad /= 2;
    }

    vida -= cantidad;

    if(vida < 0){
        vida = 0;
    }
}

void Jugador::obtenerArmadura()
{
    tieneArmadura = true;

    fuerzaSalto = 12;
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

float Jugador::getFuerzaSalto() const
{
    return fuerzaSalto;
}

bool Jugador::getEnSuelo() const
{
    return enSuelo;
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
