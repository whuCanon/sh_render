#ifndef SH_GRAPHICS_H
#define SH_GRAPHICS_H

#include <map>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "sh_geometry.h"


using namespace std;
using namespace Eigen;


namespace shr
{
    struct Texture
    {
        GLuint id;
        string type;
        aiString path;
    };


    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const vector<Vertex>& v, const vector<GLuint>& i, const vector<Texture>& t);

        void setupMesh();
        void draw(GLuint pgm);
        bool rayHitObject(const Ray &ray);

        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

    private:
        GLuint vbo_, vao_, ebo_;
    };


    class Model{
        static map<string, Texture> loaded_mesh_;
    public:
        Model() = default;
        Model(string path) { loadModel(path); }

        const BoundaryBox &bound() { return bound_; }
        const vector<Mesh> &mesh() { return meshes_; }

        void draw(GLuint pgm);

    private:
        void loadModel(string path);
        void procNode(aiNode* node, const aiScene* scene);
        Mesh procMesh(aiMesh* mesh, const aiScene* scene);
        vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string type_name);

        // create a plane under the model for shadow mapping.
        // This plane is directly below the model and area = (4*height+length) * (4*height+width),
        //     which assumes angle of shadow from light more than 60 degree can be ignored.
        // The variable z of plane will be transfered into frag_shader as a flag to decide
        //     if the vertex needs to be viewed as a plane vertex.
        void createPlane();

        void generateRenderCoeffs();
        bool isRayBlocked(const Ray &ray);

        vector<Mesh> meshes_;
        BoundaryBox  bound_;
        string dir_;
    };

    GLuint            createProgram(vector<tuple<string, GLenum>> shadernames);
    GLuint            createProgram(string vertexfilename, string fragmentfilename);
    GLuint            loadTexture(string filename);
    shared_ptr<Model> loadModel(string filename);
}

#endif // SH_GRAPHICS_H
