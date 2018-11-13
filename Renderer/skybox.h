#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <Eigen/Dense>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace Eigen;


namespace shr
{
    class Skybox
    {
    public:
        Skybox(string pano_image);
        ~Skybox();

        void draw(const Matrix4f &viewproj);

    private:
        Skybox(const Skybox&) = delete;
        void operator=(const Skybox&) = delete;

        GLuint cubemap_;       // skybox panorama image
        GLuint cube_;     // skybox sphere
        GLuint program_;
    };

    array<string, 6> panoToCubemap(const string &pano);
    GLuint loadCubemap(array<string, 6> facefiles);
    GLuint createSkyboxProgram();
    GLuint createSkybox();
}

#endif // SKYBOX_H
