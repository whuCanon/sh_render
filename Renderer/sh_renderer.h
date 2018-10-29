#ifndef SH_RENDERER_H
#define SH_RENDREER_H

#include <vector>
#include <string>
#include <iostream>
#include <FreeImage.h>

#include "sh_background.h"
#include "sh_geometry.h"
#include "sh_graphics.h"
#include "inputs.h"

using namespace std;
using namespace Eigen;


// spherical harmonics rendering framework
namespace shr
{
    class InputProcessor;

    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(int order, const vector<float> &coeffs) : order_(order), coeffs_(coeffs) {}
        virtual ~Renderer() = default;

        int    getWindowWidth() const { return window_width_; }
        int    getWindowHeight() const { return window_height_; }
        string getWindowTitle() const { return title_; }
        void   setWindowTitle(string title) { title_ = title; }
        void   setWindowSize(int w, int h) { window_width_ = w; window_height_ = h; }
        void   setInputProcessor(InputProcessor* p) { input_processor_ = p; }
        void   setSHCoeffs(int order, vector<float> &coeffs) { order_ = order; coeffs_ = coeffs; }

        virtual void onInit() = 0;
        virtual void onUpdate(float dt) = 0;
        virtual void onShutdown() = 0;

        void run();
        float frameRatio();

    private:
        Renderer(const Renderer &) = delete;
        void operator=(const Renderer &) = delete;

        void init();
        void shutdown();
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseCallback(GLFWwindow* window, double posx, double posy);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseWheelCallback(GLFWwindow* window, double offset_x, double offset_y);

        static Renderer* instance_;
        int              order_;
        vector<float>   coeffs_;
        string           title_ = "Spherical Harmonics Lighting";
        int              window_width_ = 640;
        int              window_height_ = 480;
        GLFWwindow*      window_ = nullptr;
        InputProcessor*  input_processor_ = nullptr;

        float pre_xpos_, pre_ypos_;
    };
}

#endif
