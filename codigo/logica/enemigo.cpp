#include "enemigo.h"
#include <cmath>

Enemigo::Enemigo(float xInicial,
                 float yInicial,
                 TipoEnemigo tipo,
                 int vida,
                 int danio)
    : Entidad(xInicial, yInicial)
{
    this->tipo = tipo;
    this->vida = vida;
    this->danio = danio;

    velocidadMovimiento = 2.0f;
    velocidadEmbiste = 4.5f;

    limiteIzquierdo = xInicial - 100.0f;
    limiteDerecho = xInicial + 100.0f;

    direccion = 1;
    rangoDeteccion = 160.0f;
    tiempoEstado = 0.0f;

    objetivoX = xInicial;
    objetivoY = yInicial;

    if(tipo == PATRULLERO)
    {
        estado = PATRULLANDO;
    }
    else
    {
        estado = QUIETO;
    }
}

void Enemigo::actualizar()
{
    if(!estaActiva() || vida <= 0)
    {
        setActiva(false);
        return;
    }

    if(tipo == PATRULLERO)
    {
        setVelocidadX(velocidadMovimiento * direccion);
        setVelocidadY(0);

        mover();

        if(getX() <= limiteIzquierdo)
        {
            setX(limiteIzquierdo);
            direccion = 1;
        }

        if(getX() >= limiteDerecho)
        {
            setX(limiteDerecho);
            direccion = -1;
        }
    }
    else if(tipo == VOLADOR || tipo == JEFE)
    {
        if(estado == EMBISTIENDO)
        {
            mover();

            tiempoEstado += 1.0f;

            if(tiempoEstado >= 20.0f)
            {
                estado = RECUPERANDO;
                tiempoEstado = 0.0f;
                setVelocidadX(0);
                setVelocidadY(0);
            }
        }
        else if(estado == RECUPERANDO)
        {
            tiempoEstado += 1.0f;

            if(tiempoEstado >= 25.0f)
            {
                estado = QUIETO;
                tiempoEstado = 0.0f;
            }
        }
        else
        {
            setVelocidadX(0);
            setVelocidadY(0);
        }
    }
}

void Enemigo::detectarJugador(float xJugador, float yJugador)
{
    if(!(tipo == VOLADOR || tipo == JEFE))
    {
        return;
    }

    if(estado != QUIETO)
    {
        return;
    }

    float dx = xJugador - getX();
    float dy = yJugador - getY();
    float distancia = std::sqrt(dx * dx + dy * dy);

    if(distancia <= rangoDeteccion && distancia > 0.0f)
    {
        objetivoX = xJugador;
        objetivoY = yJugador;

        setVelocidadX((dx / distancia) * velocidadEmbiste);
        setVelocidadY((dy / distancia) * velocidadEmbiste);

        estado = EMBISTIENDO;
        tiempoEstado = 0.0f;
    }
}

void Enemigo::recibirDanio(int cantidad)
{
    vida -= cantidad;

    if(vida < 0)
    {
        vida = 0;
    }

    if(vida == 0)
    {
        setActiva(false);
    }
}

void Enemigo::setLimites(float izquierdo, float derecho)
{
    limiteIzquierdo = izquierdo;
    limiteDerecho = derecho;
}

void Enemigo::setRangoDeteccion(float rango)
{
    rangoDeteccion = rango;
}

TipoEnemigo Enemigo::getTipo() const
{
    return tipo;
}

EstadoEnemigo Enemigo::getEstado() const
{
    return estado;
}

int Enemigo::getVida() const
{
    return vida;
}

int Enemigo::getDanio() const
{
    return danio;
}

bool Enemigo::estaVivo() const
{
    return vida > 0 && estaActiva();
}