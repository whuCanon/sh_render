SOURCES += \
    main.cpp \
    Renderer/sh_renderer.cpp \
    Renderer/sh_geometry.cpp \
    Renderer/sh_graphics.cpp \
    Renderer/inputs.cpp \
    Renderer/sh_background.cpp

HEADERS += \
    Renderer/sh_graphics.h \
    Renderer/sh_geometry.h \
    Renderer/sh_renderer.h \
    Renderer/inputs.h \
    Renderer/sh_background.h \
    Renderer/common.h

CONFIG += c++11


unix: CONFIG += link_pkgconfig

unix: PKGCONFIG += /usr/local/lib/x86_64-linux-gnu/pkgconfig/glm.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/glew.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/glfw3.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/assimp.pc

unix: LIBS += -L/usr/local/lib/ -lfreeimage
INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include

unix: LIBS += -L/usr/local/lib/ -lsh
INCLUDEPATH += /usr/local/include/spherical_harmonics
DEPENDPATH += /usr/local/include/spherical_harmonics

unix:LIBS += -lGL

unix:INCLUDEPATH += /usr/local/include/eigen3
