#include <array>
#include <iostream>
#include <fstream>

#include "Renderer/skybox.h"
#include "Renderer/framework.h"

using namespace std;
using namespace shr;


class SHLightingApp : public shr::Renderer
{
public:
    SHLightingApp(string pano, string shl_coef, string obj)
        :pano_file_(pano), shl_coef_file_(shl_coef), objfile_(obj) {}
    ~SHLightingApp() override = default;

private:
    void onInit() override;
    void onUpdate(float dt) override;
    void onShutdown() override;

    // parameters
    string pano_file_;      // panorama file
    string shl_coef_file_;  // sh-light coeffs file
    string objfile_;        // model file

    // objects
    Skybox* skybox_;
    shared_ptr<shr::Model> model_;
    GLuint model_program_;

    float light_coeffs_[SH_NUM];
    float rotated_light_coeffs_[SH_NUM];

    shr::ObserverInput* input_proc_;

    Matrix4f view_, proj_;
};

void SHLightingApp::onInit()
{
    // load or resampling sh coeffs parameters
    ifstream ifs(shl_coef_file_);
    if (!ifs)
        throw runtime_error("open " + shl_coef_file_ + " failed");
    for (int i = 0; i < SH_NUM; i++)
        ifs >> light_coeffs_[i];

    // input proc
    input_proc_ = new shr::ObserverInput(Vector3f(1,0,0), Vector3f(0,1,0));
    this->setInputProcessor(input_proc_);

    // sky box
    skybox_ = new Skybox(pano_file_);

    // sh-light proc
    shr::lightRotate(input_proc_->getCameraView(), light_coeffs_, &light_coeffs_);

    // object
    string vert_shader = string("../data/sh_lighting_vert_") + to_string(L) + ".glsl";
    string frag_shader = string("../data/sh_lighting_frag_") + to_string(L) + ".glsl";
    model_ = shr::loadModel(objfile_);
    model_program_ = shr::createProgram(vert_shader, frag_shader);

    // setup opengl
    glViewport(0, 0, getWindowWidth(), getWindowHeight());
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SHLightingApp::onUpdate(float dt)
{
    glEnable(GL_DEPTH_TEST);

    view_ = input_proc_->getCameraView();
    proj_ = shr::perspective(60.f, frameRatio(), 0.1f, 100.f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    skybox_->draw(proj_*view_);
    glUseProgram(model_program_);

    // compute model transforms
    Matrix4f model_trans = input_proc_->getModelTransform();
    Matrix4f model_view_proj = proj_ * view_ * model_trans;

    // compute light transforms
    shr::lightRotate(model_trans.inverse(), light_coeffs_, &rotated_light_coeffs_);

    // compute plane transparent coeffs
    float trans_coeff = 0;
    for (int i = 0; i < SH_NUM; i++)
        trans_coeff += rotated_light_coeffs_[i] * model_->mesh().back().vertices[0].render_coeffs[i];

    // delivery transforms
    glUniform1f(glGetUniformLocation(model_program_, "ymin"), model_->bound().ymin());
    glUniform1f(glGetUniformLocation(model_program_, "basic_coeff"), trans_coeff);

    glUniformMatrix4fv(glGetUniformLocation(model_program_, "model_view_proj"), \
        1, false, model_view_proj.data());
    glUniform1fv(glGetUniformLocation(model_program_, "light_coeffs"), \
        SH_NUM, rotated_light_coeffs_);

    model_->draw(model_program_);

}

void SHLightingApp::onShutdown()
{
    delete input_proc_;
    delete skybox_;
    glDeleteProgram(model_program_);
}

int main(int argc, char *argv[])
{
    try{
        if (argc < 4)
            throw runtime_error("Usage: sh_lighting panorama coefficients.txt model");

        string pano_file = argv[1];
        string sh_coef_file = argv[2];
        string objfile = argv[3];

        SHLightingApp app(pano_file, sh_coef_file, objfile);
        app.setWindowSize(800, 600);
        app.setWindowTitle("Spherical Harmonics Lighting");
        app.run();
    }
    catch (exception e){
        cerr << e.what() << endl;
    }
    return 0;
}
