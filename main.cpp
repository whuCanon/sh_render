#include <array>
#include <iostream>
#include <fstream>

#include "Renderer/sh_renderer.h"
#include "Renderer/sh_geometry.h"
#include "Renderer/sh_graphics.h"
#include "Renderer/inputs.h"

using namespace std;
using namespace shr;


class SHLightingApp : public shr::Renderer
{
public:
    SHLightingApp(string bkg, string shl_coef, string obj)
        :bkg_file_(bkg), shl_coef_file_(shl_coef), objfile_(obj) {}
    ~SHLightingApp() override = default;

private:
    void onInit() override;
    void onUpdate(float dt) override;
    void onShutdown() override;

    // parameters
    //array<string, 6> cube_textures_;
    string bkg_file_;       // background image file
    string shl_coef_file_;  // sh-light coeffs file
    string objfile_;        // model file

    // objects
    //fw::SkyBox* skybox_;
    shr::Background* background_;
    shared_ptr<shr::Model> model_;
    GLuint model_program_;

    float light_coeffs_[SH_NUM];
    float rotated_light_coeffs_[SH_NUM];

    shr::ObserverInput* input_proc_;

    Matrix4f view_, proj_;
    GLuint bkg_textures_;
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
    input_proc_ = new shr::ObserverInput({ 3, 3, 3 }, { 0, 1, 0 });
    this->setInputProcessor(input_proc_);

    // sky box
    //skybox_ = new fw::SkyBox(cube_textures_);

    // background
    background_ = new shr::Background(bkg_file_);

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
}

void SHLightingApp::onUpdate(float dt)
{
    glEnable(GL_DEPTH_TEST);

    view_ = input_proc_->getCameraView();
    proj_ = shr::perspective(60, frameRatio(), 0.1f, 100);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    //skybox_->Draw(proj*view);
    background_->draw();
    glUseProgram(model_program_);

    // compute model transforms
    Matrix4f model_trans = input_proc_->getModelTransform();
    Matrix4f model_view_proj = proj_ * view_ * model_trans;

    // compute light transforms
    Matrix3f temp_trans;
    temp_trans << model_trans(0,0), model_trans(0,1), model_trans(0,2),
                  model_trans(1,0), model_trans(1,1), model_trans(1,2),
                  model_trans(2,0), model_trans(2,1), model_trans(2,2);
    Quaterniond light_trans(temp_trans.cast<double>().inverse());
    unique_ptr<sh::Rotation> light_rotation = sh::Rotation::Create(L, light_trans.normalized());
    vector<float> temp_coeffs, temp_rotated_coeffs;
    for (int i = 0; i < SH_NUM; i++)
        temp_coeffs.push_back(light_coeffs_[i]);
    light_rotation->Apply(temp_coeffs, &temp_rotated_coeffs);
    for (int i = 0; i < SH_NUM; i++)
        rotated_light_coeffs_[i] = temp_rotated_coeffs[i];

    // delivery transforms
    glUniformMatrix4fv(glGetUniformLocation(model_program_, "model_view_proj"),
        1, false, model_view_proj.data());
    glUniform1fv(glGetUniformLocation(model_program_, "light_coeffs"),
        SH_NUM, rotated_light_coeffs_);
    model_->draw(model_program_);

}

void SHLightingApp::onShutdown()
{
    delete input_proc_;
    //delete skybox_;
    glDeleteTextures(1, &bkg_textures_);
    glDeleteProgram(model_program_);
}

int main(int argc, char *argv[])
{
    try{
        if (argc < 4)
            throw runtime_error("Usage: sh_lighting image coefficients.txt model");

        string bkg_file = argv[1];
        string sh_coef_file = argv[2];
        string objfile = argv[3];

        SHLightingApp app(bkg_file, sh_coef_file, objfile);
        app.setWindowSize(800, 600);
        app.setWindowTitle("Spherical Harmonics Lighting");
        app.run();
    }
    catch (exception e){
        cerr << e.what() << endl;
    }
    return 0;
}
