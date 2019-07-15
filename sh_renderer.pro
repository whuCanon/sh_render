SOURCES += \
    Renderer/inputs.cpp \
    Renderer/background.cpp \
    Renderer/geometry.cpp \
    Renderer/graphics.cpp \
    Renderer/framework.cpp \
    Renderer/skybox.cpp \
    renderer.cpp \
    test.cpp \
    pano_viewer.cpp \
    Renderer/sh_sampler.cpp

HEADERS += \
    Renderer/inputs.h \
    Renderer/common.h \
    Renderer/skybox.h \
    Renderer/background.h \
    Renderer/geometry.h \
    Renderer/graphics.h \
    Renderer/framework.h \
    Renderer/sh_sampler.h

CONFIG += c++11


unix: CONFIG += link_pkgconfig

unix: PKGCONFIG += /usr/local/lib/x86_64-linux-gnu/pkgconfig/glm.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/glew.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/glfw3.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/assimp.pc
unix: PKGCONFIG += /usr/local/lib/pkgconfig/opencv.pc

#unix: LIBS += -L/usr/local/lib/ -lfreeimage
#INCLUDEPATH += /usr/local/include
#DEPENDPATH += /usr/local/include

unix: LIBS += -L/usr/local/lib/ -lsh
INCLUDEPATH += /usr/local/include/spherical_harmonics
DEPENDPATH += /usr/local/include/spherical_harmonics

unix:LIBS += -lGL

unix:INCLUDEPATH += /usr/local/include/eigen3

DISTFILES += \
    data/sh_lighting_frag_4.glsl \
    data/sh_lighting_vert_4.glsl \
    data/frag_shader.glsl \
    data/vert_shader.glsl
