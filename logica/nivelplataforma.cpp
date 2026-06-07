#include "nivelplataforma.h"

PlataformaNivel1::PlataformaNivel1(const Plataforma& plataforma, bool piso, bool resbalosa)
    : plataforma(plataforma),
    piso(piso),
    resbalosa(resbalosa)
{
}

NivelPlataforma::NivelPlataforma()
{
}

NivelPlataforma::NivelPlataforma(int dificultad,
                                 int anchoPantalla,
                                 int fondosRunner,
                                 int altoPantalla,
                                 int mundoAlto)
{
    cargarNivel(dificultad, anchoPantalla, fondosRunner, altoPantalla, mundoAlto);
}

void NivelPlataforma::actualizar()
{
}

void NivelPlataforma::agregarEntrada(const Plataforma& plataforma, bool piso, bool resbalosa)
{
    agregarPlataforma(plataforma);
    plataformasNivel1.emplace_back(plataforma, piso, resbalosa);
}

void NivelPlataforma::cargarNivel(int dificultad,
                                  int anchoPantalla,
                                  int fondosRunner,
                                  int altoPantalla,
                                  int mundoAlto)
{
    plataformas.clear();
    obstaculos.clear();
    plataformasNivel1.clear();
    setDificultad(dificultad);

    const int pisoBaseY = mundoAlto - altoPantalla;
    const int columnaEscaladaX = anchoPantalla * (fondosRunner - 1);
    const bool plataformasConHielo = dificultad == 2;

    auto agregar = [&](float x, float y, float ancho, float alto, bool piso)
    {
        agregarEntrada(Plataforma(x, y, ancho, alto),
                       piso,
                       plataformasConHielo && !piso);
    };

    auto yBase = [&](float yLocal) -> float
    {
        return pisoBaseY + yLocal;
    };

    auto yEscalada = [&](int fondo, float yLocal) -> float
    {
        return pisoBaseY - altoPantalla * (fondo - fondosRunner) + yLocal;
    };

    auto xEscalada = [&](float xLocal) -> float
    {
        return columnaEscaladaX + xLocal;
    };

    agregar(0, yBase(550), anchoPantalla * fondosRunner, 50, true);

    agregar(xEscalada(590), yBase(450), 130, 25, false);
    agregar(xEscalada(410), yBase(350), 130, 25, false);
    agregar(xEscalada(230), yBase(250), 130, 25, false);
    agregar(xEscalada(80),  yBase(150), 130, 25, false);
    agregar(xEscalada(360), yBase(50),  130, 25, false);

    for(int fondo = 4; fondo <= 11; fondo++)
    {
        const bool derecha = (fondo % 2 == 0);

        agregar(xEscalada(derecha ? 120 : 550), yEscalada(fondo, 550), 140, 25, false);
        agregar(xEscalada(derecha ? 340 : 330), yEscalada(fondo, 450), 140, 25, false);
        agregar(xEscalada(derecha ? 560 : 110), yEscalada(fondo, 350), 140, 25, false);
        agregar(xEscalada(derecha ? 360 : 350), yEscalada(fondo, 250), 140, 25, false);
        agregar(xEscalada(derecha ? 130 : 560), yEscalada(fondo, 150), 140, 25, false);
        agregar(xEscalada(derecha ? 430 : 250), yEscalada(fondo, 50),  140, 25, false);
    }

    agregar(columnaEscaladaX + 300, 560, 500, 50, true);
}

const std::vector<PlataformaNivel1>& NivelPlataforma::getPlataformasNivel1() const
{
    return plataformasNivel1;
}

std::vector<PlataformaNivel1> NivelPlataforma::crearPlataformasNivel1(int dificultad,
                                                                      int anchoPantalla,
                                                                      int fondosRunner,
                                                                      int altoPantalla,
                                                                      int mundoAlto)
{
    NivelPlataforma nivel(dificultad,
                          anchoPantalla,
                          fondosRunner,
                          altoPantalla,
                          mundoAlto);
    return nivel.getPlataformasNivel1();
}
