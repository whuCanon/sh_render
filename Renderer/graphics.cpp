#include <fstream>
#include <opencv2/opencv.hpp>

#include "framework.h"

using namespace shr;
using namespace std;
using namespace Eigen;


map<string, Texture> Model::loaded_mesh_;


GLuint shr::createProgram(string vertexfilename, string fragmentfilename)
{
    std::vector<std::tuple<std::string, GLenum>> shaders;
    shaders.push_back(std::tuple<std::string, GLenum>(vertexfilename, GL_VERTEX_SHADER));
    shaders.push_back(std::tuple<std::string, GLenum>(fragmentfilename, GL_FRAGMENT_SHADER));
    return createProgram(shaders);
}


GLuint shr::createProgram(vector<tuple<string, GLenum>> shadernames)
{
    // create program
    GLuint program = glCreateProgram();
    vector<GLuint> shaders;
    // setup shader deletion
    auto delete_shaders = [](const vector<GLuint>& shaders){
        for (auto s : shaders){
            glDeleteShader(s);
        }
    };
    // setup compile error checker
    auto check_compile = [](GLint shader, std::string* info)->bool{
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE){
            // get info size
            GLint logsize = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

            // get info
            GLchar* errinfo = new GLchar[logsize];
            glGetShaderInfoLog(shader, logsize, &logsize, errinfo);
            *info = std::string(errinfo, errinfo + logsize);
            delete[] errinfo;
            return false;
        }
        return true;
    };

    // read each shader
    for (auto shaderinfo : shadernames)
    {
        string sname = get<0>(shaderinfo);
        GLenum type = get<1>(shaderinfo);
        // open file stream
        std::ifstream sfile(sname);
        if (!sfile)
            throw runtime_error("read " + sname + " failed");

        // read content
        std::string content;
        std::string line;
        while (std::getline(sfile, line))
            content += line + '\n';

        // create shader
        GLuint shader = glCreateShader(type);
        shaders.push_back(shader);
        const GLchar* str = content.c_str();
        glShaderSource(shader, 1, &str, nullptr);
        glCompileShader(shader);

        // check compile result
        std::string info;
        if (!check_compile(shader, &info)){
            delete_shaders(shaders);
            glDeleteProgram(program);
            throw runtime_error("A error ocurred when compile "
                + sname + ":\n" + info);
        }
        // attach to program
        glAttachShader(program, shader);
    }

    // link
    glLinkProgram(program);

    // clean up
    delete_shaders(shaders);
    return program;
}


GLuint shr::loadTexture(string filename)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    cv::Mat img = cv::imread(filename, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);
    int w = img.cols;
    int h = img.rows;
//    // load image
//    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(filename.c_str(), 0);//Automatocally detects the format(from over 20 formats!)
//    FIBITMAP* imagen = FreeImage_Load(formato, filename.c_str());

//    FIBITMAP* temp = imagen;
//    imagen = FreeImage_ConvertTo32Bits(imagen);
//    FreeImage_Unload(temp);

//    int w = static_cast<int>(FreeImage_GetWidth(imagen));
//    int h = static_cast<int>(FreeImage_GetHeight(imagen));
//    //cout<<"The size of the image is: "<<textureFile<<" es "<<w<<"*"<<h<<endl; //Some debugging code

//    char* pixeles = reinterpret_cast<char*>(FreeImage_GetBits(imagen));
//    //FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    if(img.type() == 16)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_BGRA, GL_FLOAT, img.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //FreeImage_Unload(imagen);
    return textureID;
}


shared_ptr<Model> shr::loadModel(string filename)
{
    shared_ptr<Model> model = std::make_shared<Model>(filename);
    return model;
}


Mesh::Mesh(const vector<Vertex> &v, const vector<GLuint> &i, const vector<Texture> &t)
{
    vertices = v;
    indices = i;
    textures = t;
    //this->setupMesh();
}

void Mesh::draw(GLuint program)
{
    // setup materials
    GLuint diffuse_count = 1;
    GLuint specular_count = 1;
    for (GLuint i = 0; i < textures.size(); i++){
        glActiveTexture(GL_TEXTURE0 + i);
        stringstream ss;
        string num;
        string type = textures[i].type;
        if (type == "texture_diffuse")
            ss << diffuse_count++;
        else if (type == "texture_specular")
            ss << specular_count++;
        num = ss.str();
        string mattype = "material." + type + num;
        GLint loc = glGetUniformLocation(program, mattype.c_str());
        glUniform1i(loc, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw
    glBindVertexArray(vao_);
    //glDrawArrays(GL_TRIANGLES, 0, indices_.size());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


bool Mesh::rayHitObject(const Ray &ray)
{
    vector<Vector3f> triangle;

    auto i = indices.begin();
    while (i != indices.end())
    {
        triangle.push_back(vertices[*i++].position);
        triangle.push_back(vertices[*i++].position);
        triangle.push_back(vertices[*i++].position);
        if (ray.isIntersectsTriangle(triangle)) return true;

        vector<Vector3f>().swap(triangle);
    }

    return false;
}


void Mesh::setupMesh()
{
    // Vertex buffer object setup
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Vertex array object setup
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Elememt buffer object setup
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    // texture coordination
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoords));
    // render coeffs
    switch (L)
    {
        case 0:
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                  (GLvoid*)(offsetof(Vertex, render_coeffs)));
            break;
        case 1:
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                  (GLvoid*)(offsetof(Vertex, render_coeffs)));
            glEnableVertexAttribArray(3+1);
            glVertexAttribPointer(3+1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                  (GLvoid*)(offsetof(Vertex, render_coeffs) + sizeof(float)));
            break;
        case 2:
            for (GLuint attrib = 0; attrib < 3; attrib++){
                glEnableVertexAttribArray(3+attrib);
                glVertexAttribPointer(3+attrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                      (GLvoid*)(offsetof(Vertex, render_coeffs) + 3*attrib*sizeof(float)));
            }
            break;
        case 3:
            for (GLuint attrib = 0; attrib < 4; attrib++){
                glEnableVertexAttribArray(3+attrib);
                glVertexAttribPointer(3+attrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                      (GLvoid*)(offsetof(Vertex, render_coeffs) + 4*attrib*sizeof(float)));
            }
            break;
        case 4:
            for (GLuint attrib = 0; attrib < 6; attrib++){
                glEnableVertexAttribArray(3+attrib);
                glVertexAttribPointer(3+attrib, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                      (GLvoid*)(offsetof(Vertex, render_coeffs) + 4*attrib*sizeof(float)));
            }
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), \
                                  (GLvoid*)(offsetof(Vertex, render_coeffs) + 24*sizeof(float)));
    }

    // detach
    glBindVertexArray(0);
}


void Model::draw(GLuint program)
{
    for (auto& mesh : meshes_)
        mesh.draw(program);
}

void Model::loadModel(string path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        throw runtime_error(string("Loading model error:") + importer.GetErrorString());
        return ;
    }
    dir_ = path.substr(0, path.find_last_of('/'));

    procNode(scene->mRootNode, scene);
    createPlane();
    generateRenderCoeffs();
    for (auto m = meshes_.begin(); m != meshes_.end(); m++)
        m->setupMesh();
}


void Model::procNode(aiNode* node, const aiScene* scene)
{
    // process meshes
    for (GLuint i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(procMesh(mesh, scene));
    }

    //--- recursively walk on node ---
    for (GLuint i = 0; i < node->mNumChildren; i++)
        procNode(node->mChildren[i], scene);
}


Mesh Model::procMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    // process vertices
    for (GLuint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v;
        const auto& mv = mesh->mVertices[i];
        if (!mesh->mNormals)
            throw runtime_error("model missing normal");
        const auto& mn = mesh->mNormals[i];

        v.position = Vector3f(mv.x, mv.y, mv.z);
        v.normal = Vector3f(mn.x, mn.y, mn.z);
        if (mesh->HasTextureCoords(0)){
            const auto& mtex = mesh->mTextureCoords[0][i];
            v.texcoords = Vector2f(mtex.x, mtex.y);
        }
        else{
            v.texcoords = Vector2f::Zero();
        }
        vertices.push_back(v);

        bound_.updateBound(v.position);
    }
    // process indices
    for (GLuint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    if (mesh->mMaterialIndex > 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        vector<Texture> diffuseMaps = loadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        vector<Texture> specularMaps = loadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}


vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // find in recoreds
        string texfile = str.C_Str();
        auto texiter = loaded_mesh_.find(texfile);
        if (texiter != loaded_mesh_.end()){
            // used loaded texture
            textures.push_back(texiter->second);
        }
        else{
            Texture texture;
            texture.id = loadTexture(dir_ + '/' + str.C_Str());
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            loaded_mesh_[texfile] = texture;
        }
    }
    return textures;
}


void Model::createPlane()
{
    float l = bound_.length();
    float w = bound_.width();
    float h = bound_.height();

    float x = bound_.xmin() - SCALE*h;
    float y = bound_.ymin() - 10*EPSILON;
    float z = bound_.zmin() - SCALE*h;

    int count = sqrt(PLANE_VNUM);
    float dx = (2*SCALE*h + l) / static_cast<float>(count);
    float dz = (2*SCALE*h + w) / static_cast<float>(count);

    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    for (int i = 0; i < count; i++){
        for (int j = 0; j < count; j++)
        {
            Vertex v;
            v.position  = Vector3f(x+i*dx, y, z+j*dz);
            v.normal    = Vector3f(0, 1, 0);
            v.texcoords = Vector2f(0, 0);
            vertices.push_back(v);
        }
    }

    for (int i = 0; i < count-1; i++){
        for (int j = 0; j < count-1; j++)
        {
            indices.push_back(static_cast<GLuint>(   i  *count +   j   ));
            indices.push_back(static_cast<GLuint>(   i  *count + (j+1) ));
            indices.push_back(static_cast<GLuint>( (i+1)*count +   j   ));
            indices.push_back(static_cast<GLuint>( (i+1)*count +   j   ));
            indices.push_back(static_cast<GLuint>(   i  *count + (j+1) ));
            indices.push_back(static_cast<GLuint>( (i+1)*count + (j+1) ));
        }
    }

    bound_.setYmin(y);
    meshes_.push_back(Mesh(vertices, indices, textures));
}


void Model::generateRenderCoeffs()
{
    bool regenerate = false;

    ifstream infile("render_coeffs.dat", ios::in | ios::binary);
    if (!infile.is_open()){
        //cout << "Unable to open render_coeffs.dat, need a few moment to regenerate coefficients...";
        regenerate = true;
    }

    // if already generate, just read from file
    if(!regenerate)
    {
        for (auto m = meshes_.begin(); m != meshes_.end(); m++)
            for (auto v = m->vertices.begin(); v != m->vertices.end(); v++)
                infile.read((char* )v->render_coeffs, SH_NUM*sizeof(float));

        infile.close();
        return;
    }

    // regenerate render coefficients of all vertices
    srand(static_cast<unsigned>(time(nullptr)));
    int sqrt_num = sqrt(static_cast<double>(SAMPLE_NUM));
    for (auto m = meshes_.begin(); m != meshes_.end(); m++)
    {
        for (auto v = m->vertices.begin(); v != m->vertices.end(); v++)
        {
            int real_sampled = 0;
            for (int i = 0; i < SAMPLE_NUM; i++)
            {
                // isometric (or random) uniform sampling on sphere
                double phi = 2*PI * ((0.5+i/sqrt_num) / sqrt_num);
                double theta = acos(1 - 2 * ((0.5+i%sqrt_num) / sqrt_num));
//                double phi = 2*PI * (rand() / double(RAND_MAX));
//                double theta = PI - acos(2*(rand()/double(RAND_MAX)) - 1);
                Vector3f sample = sh::ToVector(phi, theta).cast<float>();

                float dot = sample.dot(v->normal);
                if (dot <= 0)    continue;   real_sampled++;

                // fill in a Ray structure for this sample and determine whether be blocked
                Ray ray(v->position + EPSILON*v->normal, sample);
                bool ray_blocked = isRayBlocked(ray);

                // add the contribution of this sample to the coefficients
                if (!ray_blocked){
                    for (int l = 0; l <= L; l++){
                        for (int m = -l; m <= l; m++){
                            float contrib = static_cast<float>(sh::EvalSH(l, m, phi, theta));
                            v->render_coeffs[sh::GetIndex(l,m)] += contrib * dot;
                        }
                    }
                }
            }
            // rescale coeffs
            for (int i = 0; i < SH_NUM; i++)
                v->render_coeffs[i] *= real_sampled;
        }
    }

    // save the coefficients to a file
    ofstream outfile("render_coeffs.dat", ios::out | ios::binary | ios::trunc);

    for (auto m = meshes_.begin(); m != meshes_.end(); m++)
        for (auto v = m->vertices.begin(); v != m->vertices.end(); v++)
            outfile.write((const char* )v->render_coeffs, SH_NUM*sizeof(float));

    outfile.close();
}


bool Model::isRayBlocked(const Ray &ray)
{
    for (auto m = meshes_.begin(); m != meshes_.end(); m++)
        if (m->rayHitObject(ray))   return true;

    return false;
}
