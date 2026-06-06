#ifndef NIVELPLATAFORMA_H
#define NIVELPLATAFORMA_H

#include "plataforma.h"

#include <vector>

struct PlataformaNivel1
{
    Plataforma plataforma;
    bool piso;
    bool resbalosa;

    PlataformaNivel1(const Plataforma& plataforma, bool piso, bool resbalosa);
};

class NivelPlataforma
{
public:
    static std::vector<PlataformaNivel1> crearPlataformasNivel1(int dificultad,
                                                                int anchoPantalla,
                                                                int fondosRunner,
                                                                int altoPantalla,
                                                                int mundoAlto);
};

#endif // NIVELPLATAFORMA_H
