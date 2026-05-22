#ifndef NIVEL_H
#define NIVEL_H

#include <vector>
#include "plataforma.h"
#include "obstaculo.h"

class Nivel
{
protected:
    std::vector<Plataforma> plataformas;
    std::vector<Obstaculo> obstaculos;

    bool completado;
    int dificultad;
public:
    Nivel();
    virtual ~Nivel();

    virtual void actualizar() = 0;

    void agregarPlataforma(const Plataforma& plataforma);
    void agregarObstaculo(const Obstaculo& obstaculo);

    const std::vector<Plataforma>& getPlataformas() const;
    const std::vector<Obstaculo>& getObstaculos() const;

    bool estaCompletado() const;
    void setCompletado(bool estado);

    int getDificultad() const;
    void setDificultad(int nuevaDificultad);
};

#endif // NIVEL_H
