QT += core gui widgets multimedia

CONFIG += c++17

SOURCES += \
    gui/menuprincipal.cpp \
    logica/meta.cpp \
    logica/proyectil.cpp \
    main.cpp \
    logica/juego.cpp \
    logica/nivel.cpp \
    logica/nivelrunner.cpp \
    logica/nivelplataforma.cpp \
    logica/entidad.cpp \
    logica/jugador.cpp \
    logica/enemigo.cpp \
    logica/obstaculo.cpp \
    logica/powerup.cpp \
    logica/agenteinteligente.cpp \
    fisica/fisica.cpp \
    fisica/movimientoparabolico.cpp \
    fisica/movimientooscilatorio.cpp \
    fisica/efectofriccion.cpp \
    logica/plataforma.cpp \
    gui/ventanajuego.cpp \
    gui/escenajuego.cpp

HEADERS += \
    gui/menuprincipal.h \
    logica/juego.h \
    logica/meta.h \
    logica/nivel.h \
    logica/nivelrunner.h \
    logica/nivelplataforma.h \
    logica/entidad.h \
    logica/jugador.h \
    logica/enemigo.h \
    logica/obstaculo.h \
    logica/powerup.h \
    logica/agenteinteligente.h \
    fisica/fisica.h \
    fisica/movimientoparabolico.h \
    fisica/movimientooscilatorio.h \
    fisica/efectofriccion.h \
    logica/plataforma.h \
    gui/ventanajuego.h \
    gui/escenajuego.h \
    logica/proyectil.h

RESOURCES += resources.qrc
