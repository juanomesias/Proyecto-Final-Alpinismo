#include "nivel.h"

Nivel::Nivel()
{
    completado = false;
    dificultad = 1;
}

Nivel::~Nivel()
{
}

void Nivel::agregarPlataforma(const Plataforma& plataforma)
{
    plataformas.push_back(plataforma);
}

void Nivel::agregarObstaculo(const Obstaculo& obstaculo)
{
    obstaculos.push_back(obstaculo);
}

const std::vector<Plataforma>& Nivel::getPlataformas() const
{
    return plataformas;
}

const std::vector<Obstaculo>& Nivel::getObstaculos() const
{
    return obstaculos;
}

bool Nivel::estaCompletado() const
{
    return completado;
}

void Nivel::setCompletado(bool estado)
{
    completado = estado;
}

int Nivel::getDificultad() const
{
    return dificultad;
}

void Nivel::setDificultad(int nuevaDificultad)
{
    dificultad = nuevaDificultad;
}