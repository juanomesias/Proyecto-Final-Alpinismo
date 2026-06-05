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
    jugadorObjetivoX(xInicial),
    jugadorObjetivoY(yInicial),
    velocidadJugadorObjetivoX(0.0f),
    velocidadJugadorObjetivoY(0.0f),
    jugadorEnAire(false),
    jugadorProtegiendo(false),
    jugadorAtacando(false),
    modoDuelo(false),
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

void Enemigo::percibirJugador(float xJugador,
                              float yJugador,
                              float velocidadJugadorX,
                              float velocidadJugadorY,
                              bool jugadorEnAire,
                              bool jugadorProtegiendo,
                              bool jugadorAtacando,
                              bool modoDuelo)
{
    jugadorObjetivoX = xJugador;
    jugadorObjetivoY = yJugador;
    velocidadJugadorObjetivoX = velocidadJugadorX;
    velocidadJugadorObjetivoY = velocidadJugadorY;
    this->jugadorEnAire = jugadorEnAire;
    this->jugadorProtegiendo = jugadorProtegiendo;
    this->jugadorAtacando = jugadorAtacando;
    this->modoDuelo = modoDuelo;

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

    ia->percibir(getX(),
                 getY(),
                 jugadorObjetivoX,
                 jugadorObjetivoY,
                 velocidadJugadorObjetivoX,
                 velocidadJugadorObjetivoY,
                 jugadorEnAire,
                 jugadorProtegiendo,
                 jugadorAtacando,
                 modoDuelo,
                 vida);
    ia->razonar();
    ia->actuar(deltaTiempo);
    ia->aprender();

    invulnerable = ia->estaInvulnerable();

    const float dx = jugadorObjetivoX - getX();
    const float dy = jugadorObjetivoY - getY();
    const float distancia = std::sqrt(dx * dx + dy * dy);
    const AtaqueIA ataque = ia->getAtaqueActual();

    if(ataque == AtaqueIA::ATAQUE_OSCILATORIO)
    {
        estado = OSCILANDO;
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    if(ataque == AtaqueIA::ATAQUE_ACELERAR_CAZA)
    {
        estado = PRESIONANDO;
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    if(ataque == AtaqueIA::ATAQUE_INVOCAR_APOYO)
    {
        estado = INVOCANDO_APOYO;
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    if(ataque == AtaqueIA::ATAQUE_GIRO ||
        ia->getEstadoActual() == EstadoIA::GOLPE_GIRATORIO)
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

    if(ia->getEstadoActual() == EstadoIA::RECUPERANDO_IA)
    {
        estado = RECUPERANDO;
        setVelocidadX(0.0f);
        setVelocidadY(0.0f);
        return;
    }

    if(ataque == AtaqueIA::ATAQUE_PROYECTIL_PREDICTIVO)
    {
        estado = QUIETO;
        lanzarProyectilDireccionado(ia->getDireccionProyectilX(),
                                    ia->getDireccionProyectilY(),
                                    ia->getVelocidadProyectilActual() - 6.0f);
        ia->consumirDecision();
        return;
    }

    if(modoDuelo)
    {
        estado = PERSIGUIENDO;
        if(distancia > 90.0f && distancia > 0.0f)
        {
            setVelocidadX((dx / distancia) * ia->getVelocidadMovimientoActual());
            setVelocidadY((dy / distancia) * (ia->getVelocidadMovimientoActual() * 0.6f));
            mover();
        }
        else
        {
            setVelocidadX(0.0f);
            setVelocidadY(0.0f);
        }
    }
    else
    {
        estado = PERSIGUIENDO;
        const float objetivoX = jugadorObjetivoX - 180.0f;
        const float objetivoY = jugadorObjetivoY - 10.0f;
        const float deltaX = objetivoX - getX();
        const float deltaY = objetivoY - getY();
        const float distanciaObjetivo = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        if(distanciaObjetivo > 4.0f && distanciaObjetivo > 0.0f)
        {
            setVelocidadX((deltaX / distanciaObjetivo) * ia->getVelocidadMovimientoActual());
            setVelocidadY((deltaY / distanciaObjetivo) * (ia->getVelocidadMovimientoActual() * 0.55f));
            mover();
        }
        else
        {
            setVelocidadX(0.0f);
            setVelocidadY(0.0f);
        }
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

        if(proyectil.getX() < -600.0f ||
            proyectil.getX() > 15000.0f ||
            proyectil.getY() < -300.0f ||
            proyectil.getY() > 1800.0f)
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

void Enemigo::consumirDecisionIA()
{
    if(ia)
        ia->consumirDecision();
}

TipoEnemigo Enemigo::getTipo() const
{
    return tipo;
}

EstadoEnemigo Enemigo::getEstado() const
{
    return estado;
}

AtaqueIA Enemigo::getAtaqueIA() const
{
    return ia ? ia->getAtaqueActual() : AtaqueIA::ATAQUE_NINGUNO;
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