#ifndef NIVELPLATAFORMA_H
#define NIVELPLATAFORMA_H

#include "nivel.h"

#include <vector>

struct PlataformaNivel1
{
    Plataforma plataforma;
    bool piso;
    bool resbalosa;

    PlataformaNivel1(const Plataforma& plataforma, bool piso, bool resbalosa);
};

class NivelPlataforma : public Nivel
{
private:
    std::vector<PlataformaNivel1> plataformasNivel1;

    void agregarEntrada(const Plataforma& plataforma, bool piso, bool resbalosa);

public:
    NivelPlataforma();
    NivelPlataforma(int dificultad,
                    int anchoPantalla,
                    int fondosRunner,
                    int altoPantalla,
                    int mundoAlto);

    void actualizar() override;
    void cargarNivel(int dificultad,
                     int anchoPantalla,
                     int fondosRunner,
                     int altoPantalla,
                     int mundoAlto);

    const std::vector<PlataformaNivel1>& getPlataformasNivel1() const;

    static std::vector<PlataformaNivel1> crearPlataformasNivel1(int dificultad,
                                                                int anchoPantalla,
                                                                int fondosRunner,
                                                                int altoPantalla,
                                                                int mundoAlto);
};

#endif // NIVELPLATAFORMA_H
