#include "skybox.h"
#include "framework.h"

using namespace std;
using namespace shr;
using namespace Eigen;


Skybox::Skybox(string pano_image)
{
    //auto textures = panoToCubemap(pano_image);
    array<string, 6> textures = {"posx.jpg", "negx.jpg", "posy.jpg", "negy.jpg", "posz.jpg", "negz.jpg"};

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
    // load pano image
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(pano.c_str(), 0);//Automatocally detects the format(from over 20 formats!)
    FIBITMAP* imagen = FreeImage_Load(formato, pano.c_str());
    if (!imagen){
        cout << "panorama is not found!" << endl;
        exit(0);
    }

    FIBITMAP* temp = imagen;
    imagen = FreeImage_ConvertTo32Bits(imagen);
    FreeImage_Unload(temp);

    uint w = FreeImage_GetWidth(imagen);
    uint h = FreeImage_GetHeight(imagen);
    FreeImage_FlipVertical(imagen);
    //FreeImage_FlipHorizontal(imagen);

    // sort in positive_x negtive_x positive_y negtive_y positive_z negtive_z
    RGBQUAD* pixel = (RGBQUAD *)malloc(sizeof(RGBQUAD));
    FIBITMAP* cube_image = FreeImage_Allocate(CUBE_RES, CUBE_RES, 32);
    array<string, 6> result = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};

    // we define the cubemap in camera view yet the panorama is in world view,
    // so camera -> world = x -> y, y -> z, z -> x.
    auto fposx = [](uint x, uint y, double delta) { return Vector3d(delta*(x+0.5) - 1,  1, delta*(y+0.5) - 1); };
    auto fnegx = [](uint x, uint y, double delta) { return Vector3d(1 - delta*(x+0.5), -1, delta*(y+0.5) - 1); };
    auto fposy = [](uint x, uint y, double delta) { return Vector3d(delta*(y+0.5) - 1, delta*(x+0.5) - 1,  1); };
    auto fnegy = [](uint x, uint y, double delta) { return Vector3d(1 - delta*(y+0.5), delta*(x+0.5) - 1, -1); };
    auto fposz = [](uint x, uint y, double delta) { return Vector3d( 1, 1 - delta*(x+0.5), delta*(y+0.5) - 1); };
    auto fnegz = [](uint x, uint y, double delta) { return Vector3d(-1, delta*(x+0.5) - 1, delta*(y+0.5) - 1); };
    function<Vector3d(uint, uint, double)> fa[6] = {fposx, fnegx, fposy, fnegy, fnegz, fposz};

    double delta = 2.0 / CUBE_RES;
    for (int i = 0; i < 6; i++)
    {
        for (uint x = 0; x < CUBE_RES; x++){
            for (uint y = 0; y < CUBE_RES; y++)
            {
                Vector3d pixel_pos = fa[i](x, y, delta);

                // in google shlib, (1,0,0)=(0,pi/2), (0,1,0)=(pi/2,pi/2), (0,-1,0)=(-pi/2,pi/2)
                double phi, theta;
                sh::ToSphericalCoords(pixel_pos.normalized(), &phi, &theta);
                if(phi < 0) phi += 2*PI;

                uint pano_x = (phi*w) / (2*PI);
                uint pano_y = (theta*h) / PI;
                FreeImage_GetPixelColor(imagen, pano_x, pano_y, pixel);
                FreeImage_SetPixelColor(cube_image, x, y, pixel);
            }
        }
        FreeImage_Save(FIF_PNG, (FIBITMAP*)cube_image, result[i].c_str());
    }

    free(pixel);
    free(cube_image);
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
        // load image
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(facefiles[i].c_str(), 0);//Automatocally detects the format(from over 20 formats!)
        FIBITMAP* imagen = FreeImage_Load(formato, facefiles[i].c_str());

        FIBITMAP* temp = imagen;
        imagen = FreeImage_ConvertTo32Bits(imagen);
        FreeImage_Unload(temp);

        int w = FreeImage_GetWidth(imagen);
        int h = FreeImage_GetHeight(imagen);
        FreeImage_FlipVertical(imagen);
        char* pixeles = (char*)FreeImage_GetBits(imagen);
        //FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
            w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixeles);
        FreeImage_Unload(imagen);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return tex;
}
