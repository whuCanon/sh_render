SOURCES += \
    Renderer/inputs.cpp \
    Renderer/background.cpp \
    Renderer/geometry.cpp \
    Renderer/graphics.cpp \
    Renderer/framework.cpp \
    Renderer/skybox.cpp \
    renderer.cpp \
    test.cpp \
    coeffs_viewer.cpp

HEADERS += \
    Renderer/inputs.h \
    Renderer/common.h \
    Renderer/skybox.h \
    Renderer/background.h \
    Renderer/geometry.h \
    Renderer/graphics.h \
    Renderer/framework.h

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
