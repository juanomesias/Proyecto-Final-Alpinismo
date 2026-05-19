#ifndef PLATAFORMA_H
#define PLATAFORMA_H

class Plataforma
{
private:
    float x;
    float y;
    float ancho;
    float alto;

public:
    Plataforma(float x,
               float y,
               float ancho,
               float alto);

    float getX() const;
    float getY() const;
    float getAncho() const;
    float getAlto() const;

    void setX(float nuevaX);
    void setY(float nuevaY);
};

#endif // PLATAFORMA_H
