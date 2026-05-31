#include "enemigo.h"
#include <cmath>
#include <algorithm>

Enemigo::Enemigo(float xInicial,
                 float yInicial,
                 TipoEnemigo tipo,
                 int vida,
                 int danio)
    : Entidad(xInicial, yInicial),
    gravedadProyectiles(0.08f)
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

    invulnerable = false;
    salvaRadialGenerada = false;

    jugadorObjetivoX = xInicial;
    jugadorObjetivoY = yInicial;

    if(tipo == PATRULLERO)
    {
        estado = PATRULLANDO;
    }
    else
    {
        estado = QUIETO;
    }

    ia = nullptr;
    if(tipo == JEFE)
    {
        ia = new AgenteInteligente(vida);
    }
}

Enemigo::~Enemigo()
{
    delete ia;
}

void Enemigo::actualizar()
{
    if(!estaActiva() || vida <= 0)
    {
        setActiva(false);
        return;
    }

    switch(tipo)
    {
    case PATRULLERO:
        actualizarPatrullero();
        break;

    case VOLADOR:
        actualizarVolador();
        break;

    case JEFE:
        actualizarJefe();
        break;
    }

    actualizarProyectiles();
}

void Enemigo::percibirJugador(float xJugador, float yJugador)
{
    jugadorObjetivoX = xJugador;
    jugadorObjetivoY = yJugador;

    if(tipo == VOLADOR && estado == QUIETO)
    {
        float dx = jugadorObjetivoX - getX();
        float dy = jugadorObjetivoY - getY();
        float distancia = std::sqrt(dx * dx + dy * dy);

        if(distancia <= rangoDeteccion && distancia > 0.0f)
        {
            setVelocidadX((dx / distancia) * velocidadEmbiste);
            setVelocidadY((dy / distancia) * velocidadEmbiste);

            estado = EMBISTIENDO;
            tiempoEstado = 0.0f;
        }
    }
}

void Enemigo::actualizarPatrullero()
{
    estado = PATRULLANDO;

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

void Enemigo::actualizarVolador()
{
    const float deltaTiempo = 1.0f / 60.0f;

    if(estado == EMBISTIENDO)
    {
        mover();
        tiempoEstado += deltaTiempo;

        if(tiempoEstado >= 0.8f)
        {
            estado = RECUPERANDO;
            tiempoEstado = 0.0f;
            setVelocidadX(0);
            setVelocidadY(0);
        }
    }
    else if(estado == RECUPERANDO)
    {
        tiempoEstado += deltaTiempo;

        if(tiempoEstado >= 1.0f)
        {
            estado = QUIETO;
            tiempoEstado = 0.0f;
        }
    }
    else
    {
        estado = QUIETO;
        setVelocidadX(0);
        setVelocidadY(0);
    }
}

void Enemigo::actualizarJefe()
{
    if(ia == nullptr)
    {
        return;
    }

    const float deltaTiempo = 1.0f / 60.0f;

    ia->percibir(
        getX(),
        getY(),
        jugadorObjetivoX,
        jugadorObjetivoY,
        vida
        );

    ia->razonar();
    ia->actuar(deltaTiempo);
    ia->aprender();

    invulnerable = ia->estaInvulnerable();

    float dx = jugadorObjetivoX - getX();
    float dy = jugadorObjetivoY - getY();

    float distancia =
        std::sqrt(dx * dx + dy * dy);

    EstadoIA estadoIA =
        ia->getEstadoActual();

    if(estadoIA == SALVA_RADIAL)
    {
        estado = ATAQUE_RADIAL;

        if(!salvaRadialGenerada)
        {
            lanzarSalvaRadial();

            salvaRadialGenerada = true;
        }

        setVelocidadX(0);
        setVelocidadY(0);

        return;
    }
    else
    {
        salvaRadialGenerada = false;
    }

    if(estadoIA == GOLPE_GIRATORIO)
    {
        estado = GIRANDO;

        if(distancia > 0.0f)
        {
            setVelocidadX(
                (dx / distancia)
                * (ia->getVelocidadMovimientoActual() * 1.4f)
                );

            setVelocidadY(
                (dy / distancia)
                * (ia->getVelocidadMovimientoActual())
                );
        }
        else
        {
            setVelocidadX(0);
            setVelocidadY(0);
        }

        mover();

        return;
    }

    if(estadoIA == RECUPERANDO_IA)
    {
        estado = RECUPERANDO;

        setVelocidadX(0);
        setVelocidadY(0);

        return;
    }

    if(ia->getAtaqueActual() == ATAQUE_PROYECTIL)
    {
        estado = QUIETO;

        lanzarProyectilDireccionado(
            ia->getDireccionProyectilX(),
            ia->getDireccionProyectilY()
            );

        ia->consumirDecision();

        return;
    }

    /*
     * MOVIMIENTO NORMAL
     */

    estado = QUIETO;

    if(distancia > 120.0f &&
        distancia > 0.0f)
    {
        setVelocidadX(
            (dx / distancia)
            * ia->getVelocidadMovimientoActual()
            );

        setVelocidadY(
            (dy / distancia)
            * (ia->getVelocidadMovimientoActual() * 0.5f)
            );

        mover();
    }
    else
    {
        setVelocidadX(0);
        setVelocidadY(0);
    }

    if(vida <= 0)
    {
        setActiva(false);
    }
}

void Enemigo::actualizarProyectiles()
{
    for(Proyectil& proyectil : proyectiles)
    {
        if(!proyectil.estaActivo())
        {
            continue;
        }

        gravedadProyectiles.aplicar(&proyectil);

        proyectil.actualizar();

        if(proyectil.getX() < -200 ||
            proyectil.getX() > 2200 ||
            proyectil.getY() < -200 ||
            proyectil.getY() > 1200)
        {
            proyectil.desactivar();
        }
    }

    proyectiles.erase(
        std::remove_if(
            proyectiles.begin(),
            proyectiles.end(),
            [](const Proyectil& p)
            {
                return !p.estaActivo();
            }
            ),
        proyectiles.end()
        );
}

void Enemigo::lanzarProyectilDireccionado(
    float direccionX,
    float direccionY
    )
{
    float magnitud =
        std::sqrt(
            direccionX * direccionX +
            direccionY * direccionY
            );

    if(magnitud <= 0.0f)
    {
        direccionX = 1.0f;
        direccionY = 0.0f;
        magnitud = 1.0f;
    }

    float dx = direccionX / magnitud;
    float dy = direccionY / magnitud;

    proyectiles.push_back(
        Proyectil(
            getX(),
            getY(),
            dx,
            dy,
            6.0f,
            danio
            )
        );
}

void Enemigo::lanzarSalvaRadial()
{
    const int cantidad = 8;
    const float pi = 3.14159265f;
    const float velocidad = 5.0f;

    for(int i = 0; i < cantidad; ++i)
    {
        float angulo = (2.0f * pi * i) / cantidad;

        float dx = std::cos(angulo);
        float dy = std::sin(angulo);

        Proyectil(
            getX(),
            getY(),
            dx,
            dy,
            velocidad,
            danio + 10
            );
    }
}

void Enemigo::recibirDanio(int cantidad)
{
    if(invulnerable)
    {
        return;
    }

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

bool Enemigo::estaInvulnerable() const
{
    return invulnerable;
}

const std::vector<Proyectil>& Enemigo::getProyectiles() const
{
    return proyectiles;
}