#include "sh_renderer.h"

using namespace std;
using namespace shr;
using namespace Eigen;


Background::Background(string textures_path)
{
    image_   = loadTexture(textures_path);
    program_ = createBkgProgram();
    square_  = createBackground();
}

Background::~Background()
{
    glDeleteProgram(program_);
    glDeleteTextures(1, &image_);
    glDeleteBuffers(1, &square_);
}

void Background::draw()
{
    glDepthFunc(GL_LEQUAL);
    glUseProgram(program_);

    glBindVertexArray(square_);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(program_, "background"), 0);
    glBindTexture(GL_TEXTURE_2D, image_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}


GLuint shr::createBkgProgram()
{
    static const GLchar* vertex_src[] =
    {
        "#version 330 core								\n"
        "layout(location = 0) in vec3 pos;              \n"
        "layout(location = 1) in vec2 tex;              \n"
        "out vec2 TexCoords;							\n"
        "void main(){									\n"
        "	gl_Position = pos.xyzz;						\n"
        "	TexCoords = tex;}       					\n"
    };
    static const GLchar* frag_src[] =
    {
        "#version 330 core								\n"
        "uniform sampler2D background;					\n"
        "in vec2 TexCoords;								\n"
        "out vec4 color;								\n"
        "void main(){									\n"
        "	color = texture(background, TexCoords);}    \n"
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


GLuint shr::createBackground()
{
    static const GLfloat vertices[] = {
        -1.0f, -1.0f, 1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f, 1.0f,  0.0f, 1.0f,
         1.0f,  1.0f, 1.0f,  1.0f, 1.0f,

        -1.0f, -1.0f, 1.0f,  0.0f, 0.0f,
         1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,  1.0f, 0.0f
    };
    GLuint vbo, vao;
    GLuint size = sizeof(vertices);

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(float)));
    glBindVertexArray(0);

    return vao;
}
