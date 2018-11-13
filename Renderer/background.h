#ifndef SH_BACKGROUND_H
#define SH_BACKGROUND_H

#include <string>
#include <Eigen/Dense>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;
using namespace Eigen;


namespace shr
{
    class Background
    {
    public:
        Background(string image_path);
        ~Background();

        void draw();

    private:
        Background(const Background&) = delete;
        void operator=(const Background&) = delete;

        GLuint image_;      // background image
        GLuint square_;     // background vertices
        GLuint program_;
    };

    GLuint createBkgProgram();
    GLuint createBackground();
}

#endif // SH_BACKGROUND_H
