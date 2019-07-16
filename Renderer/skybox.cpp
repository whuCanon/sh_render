#include <opencv2/opencv.hpp>

#include "skybox.h"
#include "framework.h"

using namespace std;
using namespace shr;
using namespace Eigen;


Skybox::Skybox(string pano_image)
{
    auto textures = panoToCubemap(pano_image);
    cubemap_ = loadCubemap(textures);
    cube_    = createSkybox();
    program_ = createSkyboxProgram();
}

Skybox::~Skybox()
{
    glDeleteProgram(program_);
    glDeleteTextures(1, &cubemap_);
    glDeleteBuffers(1, &cube_);
}

void Skybox::draw(const Matrix4f &viewproj)
{
    // draw skybox
    // note: the sky box depth is always 1
    glDepthFunc(GL_LEQUAL);
    glUseProgram(program_);

    Affine3f view(viewproj);
    Affine3f s(Scaling(10000.0f));
    glUniformMatrix4fv(glGetUniformLocation(program_, "viewproj"), \
                       1, GL_FALSE, (viewproj*s).data());

    glBindVertexArray(cube_);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program_, "skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}


GLuint shr::createSkyboxProgram()
{
    static const GLchar* vertex_src[] =
    {
        "#version 330 core								\n"
        "layout(location = 0) in vec3 position;			\n"
        "uniform mat4 viewproj;							\n"
        "out vec3 TexCoords;							\n"
        "void main(){									\n"
        "	vec4 pos = viewproj * vec4(position, 1);	\n"
        "	gl_Position = pos.xyww;						\n"
        "	TexCoords = position;}						\n"
    };
    static const GLchar* frag_src[] =
    {
        "#version 330 core								\n"
        "uniform samplerCube skybox;					\n"
        "in vec3 TexCoords;								\n"
        "out vec4 color;								\n"
        "void main(){									\n"
        "	color = texture(skybox, TexCoords);}		\n"
    };

    // Create and compile vertex shader
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_src, NULL);
    glCompileShader(vertex_shader);

    // Create and compile fragment shader
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, frag_src, NULL);
    glCompileShader(fragment_shader);

    // Create program, attach shaders to it, and link it
    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // Delete the shaders as the program has them now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}


GLuint shr::createSkybox()
{
    static const GLfloat cube[] = {
        // six faces vertices
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f, 1.0f, -1.0f,
         1.0f, 1.0f, -1.0f,
         1.0f, 1.0f,  1.0f,
         1.0f, 1.0f,  1.0f,
        -1.0f, 1.0f,  1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
    };

    GLuint vbo, vao;
    GLuint size = sizeof(cube);

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBufferData(GL_ARRAY_BUFFER, size, cube, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    return vao;
}


array<string, 6> shr::panoToCubemap(const string &pano)
{
    cv::Mat img = cv::imread(pano, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    int w = img.cols;
    int h = img.rows;
    int type = img.type();

    cv::Mat cubemap = cv::Mat::zeros(CUBE_RES, CUBE_RES, type);
    array<string, 6> result;
    cv::flip(img, img, -1);
    if(type == 16)
        result = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};
    else
        result = {"posx.exr", "negx.exr", "posy.exr", "negy.exr", "posz.exr", "negz.exr"};

    // we define the cubemap in camera view yet the panorama is in world view,
    // so camera -> world = x -> y, y -> z, z -> x.
    auto fposx = [](int x, int y, double delta) { return Vector3d(1 - delta*(x+0.5), -1, delta*(y+0.5) - 1); };
    auto fnegx = [](int x, int y, double delta) { return Vector3d(delta*(x+0.5) - 1,  1, delta*(y+0.5) - 1); };
    auto fposy = [](int x, int y, double delta) { return Vector3d(1 - delta*(y+0.5), delta*(x+0.5) - 1, -1); };
    auto fnegy = [](int x, int y, double delta) { return Vector3d(delta*(y+0.5) - 1, delta*(x+0.5) - 1,  1); };
    auto fposz = [](int x, int y, double delta) { return Vector3d( 1, 1 - delta*(x+0.5), delta*(y+0.5) - 1); };
    auto fnegz = [](int x, int y, double delta) { return Vector3d(-1, delta*(x+0.5) - 1, delta*(y+0.5) - 1); };
    function<Vector3d(int, int, double)> fa[6] = {fposx, fnegx, fposy, fnegy, fnegz, fposz};

    double delta = 2.0 / CUBE_RES;
    for (int i = 0; i < 6; i++)
    {
        for (int x = 0; x < CUBE_RES; x++){
            for (int y = 0; y < CUBE_RES; y++)
            {
                Vector3d pixel_pos = fa[i](x, y, delta);

                // in google shlib, (1,0,0)=(0,pi/2), (0,1,0)=(pi/2,pi/2), (0,-1,0)=(-pi/2,pi/2)
                double phi, theta;
                sh::ToSphericalCoords(pixel_pos.normalized(), &phi, &theta);
                if(phi < 0) phi += 2*PI;

                int pano_x = (phi*w) / (2*PI);
                int pano_y = (theta*h) / PI;
                if(type == 16)
                    cubemap.at<cv::Vec3b>(y,x) = img.at<cv::Vec3b>(pano_y, pano_x);
                else
                    cubemap.at<cv::Vec3f>(y,x) = img.at<cv::Vec3f>(pano_y, pano_x);
            }
        }
        cv::imwrite(result[i], cubemap);
    }
    return result;
}


GLuint shr::loadCubemap(array<string, 6> facefiles)
{
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    for (unsigned int i = 0; i < facefiles.size(); i++)
    {
        cv::Mat img = cv::imread(facefiles[i], cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
        int w = img.cols;
        int h = img.rows;
        cv::flip(img, img, 1);

        if(img.type() == 16)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);
        else
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                w, h, 0, GL_BGR, GL_FLOAT, img.data);
        //FreeImage_Unload(imagen);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return tex;
}
