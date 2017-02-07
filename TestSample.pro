TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ./src/main.c \
    ./src/ipc.c \
    ./src/network.c \
    ./src/udp_server_sim.c \
    ./src/ux_sim.c \
    ./src/led.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ./src/ipc.h \
    ./src/common.h \
    ./src/network.h \
    ./src/led.h


SOURCES -= ./src/udp_server_sim.c \
        ./src/ux_sim.c

DISTFILES += \
    Makefile

