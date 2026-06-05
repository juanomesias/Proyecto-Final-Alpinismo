#include "enemigo.h"

#include <algorithm>
#include <cmath>

Enemigo::Enemigo(float xInicial,
                 float yInicial,
                 TipoEnemigo tipo,
                 int vida,
                 int danio)
    : Entidad(xInicial, yInicial),
    tipo(tipo),
    estado(tipo == PATRULLERO ? PATRULLANDO : QUIETO),
    vida(vida),
    danio(danio),
    velocidadMovimiento(2.2f),
    velocidadEmbiste(5.0f),
    limiteIzquierdo(xInicial - 100.0f),
    limiteDerecho(xInicial + 100.0f),
    direccion(1),
    rangoDeteccion(180.0f),
    tiempoEstado(0.0f),
    invulnerable(false),
    salvaRadialGenerada(false),
    jugadorObjetivoX(xInicial),
    jugadorObjetivoY(yInicial),
    gravedadProyectiles(0.0f)
{
    if(tipo == JEFE)
        ia = std::make_unique<AgenteInteligente>(vida);
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
        const float dx = jugadorObjetivoX - getX();
        const float dy = jugadorObjetivoY - getY();
        const float distancia = std::sqrt(dx * dx + dy * dy);

        if(distancia <= rangoDeteccion && distancia > 0.0f)
        {
            setVelocidadX((dx / distancia) * velocidadEmbiste);
            setVelocidadY((dy / distancia) * (velocidadEmbiste * 0.7f));
            estado = EMBISTIENDO;
            tiempoEstado = 0.0f;
        }
    }
}

void Enemigo::actualizarPatrullero()
{
    estado = PATRULLANDO;
    setVelocidadX(velocidadMovimiento * direccion);
    setVelocidadY(0.0f);
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
            setVelocidadX(0.0f);
            setVelocidadY(0.0f);
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
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
    }
}

void Enemigo::actualizarJefe()
{
    if(!ia)
        return;

    const float deltaTiempo = 1.0f / 60.0f;

    ia->percibir(getX(), getY(), jugadorObjetivoX, jugadorObjetivoY, vida);
    ia->razonar();
    ia->actuar(deltaTiempo);
    ia->aprender();

    invulnerable = ia->estaInvulnerable();

    const float dx = jugadorObjetivoX - getX();
    const float dy = jugadorObjetivoY - getY();
    const float distancia = std::sqrt(dx * dx + dy * dy);
    const EstadoIA estadoIA = ia->getEstadoActual();

    if(estadoIA == SALVA_RADIAL)
    {
        estado = ATAQUE_RADIAL;
        if(!salvaRadialGenerada)
        {
            lanzarSalvaRadial();
            salvaRadialGenerada = true;
        }

        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    salvaRadialGenerada = false;

    if(estadoIA == GOLPE_GIRATORIO)
    {
        estado = GIRANDO;
        if(distancia > 0.0f)
        {
            setVelocidadX((dx / distancia) * (ia->getVelocidadMovimientoActual() * 1.35f));
            setVelocidadY((dy / distancia) * ia->getVelocidadMovimientoActual());
        }
        else
        {
            setVelocidadX(0.0f);
            setVelocidadY(0.0f);
        }

        mover();
        return;
    }

    if(estadoIA == RECUPERANDO_IA)
    {
        estado = RECUPERANDO;
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    if(ia->getAtaqueActual() == ATAQUE_PROYECTIL)
    {
        estado = QUIETO;
        lanzarProyectilDireccionado(ia->getDireccionProyectilX(),
                                    ia->getDireccionProyectilY(),
                                    ia->getVelocidadProyectilActual() - 6.0f);
        ia->consumirDecision();
        return;
    }

    estado = QUIETO;

    if(distancia > 120.0f && distancia > 0.0f)
    {
        setVelocidadX((dx / distancia) * ia->getVelocidadMovimientoActual());
        setVelocidadY((dy / distancia) * (ia->getVelocidadMovimientoActual() * 0.45f));
        mover();
    }
    else
    {
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
    }
}

void Enemigo::actualizarProyectiles()
{
    for(Proyectil& proyectil : proyectiles)
    {
        if(!proyectil.estaActivo())
            continue;

        gravedadProyectiles.aplicar(&proyectil);
        proyectil.actualizar();

        if(proyectil.getX() < -300.0f ||
            proyectil.getX() > 6000.0f ||
            proyectil.getY() < -200.0f ||
            proyectil.getY() > 1400.0f)
        {
            proyectil.desactivar();
        }
    }

    proyectiles.erase(
        std::remove_if(
            proyectiles.begin(),
            proyectiles.end(),
            [](const Proyectil& proyectil)
            {
                return !proyectil.estaActivo();
            }),
        proyectiles.end());
}

void Enemigo::lanzarProyectilDireccionado(float direccionX, float direccionY, float velocidadExtra)
{
    float magnitud = std::sqrt(direccionX * direccionX + direccionY * direccionY);
    if(magnitud <= 0.0f)
    {
        direccionX = 1.0f;
        direccionY = 0.0f;
        magnitud = 1.0f;
    }

    const float dx = direccionX / magnitud;
    const float dy = direccionY / magnitud;

    proyectiles.emplace_back(getX(),
                             getY(),
                             dx,
                             dy,
                             6.0f + velocidadExtra,
                             danio);
}

void Enemigo::lanzarSalvaRadial()
{
    const int cantidad = 8;
    const float pi = 3.14159265f;
    const float velocidad = ia ? ia->getVelocidadProyectilActual() : 5.0f;

    for(int i = 0; i < cantidad; ++i)
    {
        const float angulo = (2.0f * pi * i) / cantidad;
        const float dx = std::cos(angulo);
        const float dy = std::sin(angulo);
        proyectiles.emplace_back(getX(), getY(), dx, dy, velocidad, danio + 10);
    }
}

void Enemigo::recibirDanio(int cantidad)
{
    if(invulnerable || cantidad <= 0)
        return;

    vida = std::max(0, vida - cantidad);
    if(vida == 0)
        setActiva(false);
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

void Enemigo::setVelocidades(float movimiento, float embiste)
{
    velocidadMovimiento = movimiento;
    velocidadEmbiste = embiste;
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

std::vector<Proyectil>& Enemigo::getProyectiles()
{
    return proyectiles;
}

const std::vector<Proyectil>& Enemigo::getProyectiles() const
{
    return proyectiles;
}