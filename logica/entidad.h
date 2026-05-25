#ifndef ENTIDAD_H
#define ENTIDAD_H

class Entidad
{
protected:
    float x;
    float y;

    float velocidadX;
    float velocidadY;

    // Estado de la entidad
    bool activa;

public:
    Entidad(float xInicial = 0, float yInicial = 0);

    virtual ~Entidad();

    virtual void actualizar();

    void mover();

    // Getters
    float getX() const;
    float getY() const;
    float getVelocidadX() const;
    float getVelocidadY() const;
    bool estaActiva() const;

    // Setters
    void setX(float nuevaX);
    void setY(float nuevaY);
    void setVelocidadX(float nuevaVelocidadX);
    void setVelocidadY(float nuevaVelocidadY);
    void setActiva(bool estado);
};

#endif // ENTIDAD_H