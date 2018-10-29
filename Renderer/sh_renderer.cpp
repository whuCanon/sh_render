#include "sh_renderer.h"

using namespace std;
using namespace shr;


Renderer* Renderer::instance_ = nullptr;

namespace{
    void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

void Renderer::run()
{
    init();
    this->onInit();
    float lasttime = glfwGetTime();

    while (!glfwWindowShouldClose(window_))
    {
        float curtime = glfwGetTime();
        float deltatime = curtime - lasttime;
        lasttime = curtime;

        glfwPollEvents();
        this->onUpdate(float(deltatime));
        glfwSwapBuffers(window_);
    }

    this->onShutdown();
    shutdown();
}


float Renderer::frameRatio()
{
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    return float(width) / float(height);
}


void Renderer::init()
{
    instance_ = this;
    bool glfw_inited = false;

    GLenum err = glfwInit();
    if (!err){
        throw runtime_error((const char*)glewGetErrorString(err));
    }

    glfw_inited = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

    // create window
    window_ = glfwCreateWindow(window_width_, window_height_, title_.c_str(), nullptr, nullptr);
    if (!window_)
        throw runtime_error("create window failed");

    glfwMakeContextCurrent(window_);

    if (glewInit() != GLEW_OK)
        throw runtime_error("init glew failed");

    // input proc
    //glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
    glfwSetMouseButtonCallback(window_, mouseButtonCallback);
    glfwSetScrollCallback(window_, mouseWheelCallback);

    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glfwSwapBuffers(window_);

    double xpos, ypos;
    glfwGetCursorPos(window_, &xpos, &ypos);
    pre_xpos_ = float(xpos);
    pre_ypos_ = float(ypos);
}


void Renderer::shutdown()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Renderer::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Renderer* instance = Renderer::instance_;
    if (instance->input_processor_)
        instance->input_processor_->onKey(key, scancode, action, mods);
}

void Renderer::mouseCallback(GLFWwindow *window, double posx, double posy)
{
    Renderer* instance = Renderer::instance_;
    if (instance->input_processor_)
        instance->input_processor_->onMouseMove(posx, posy);
}

void Renderer::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    Renderer* instance = Renderer::instance_;
    if (instance->input_processor_)
        instance->input_processor_->onMouseButton(button, action, mods);
}

void Renderer::mouseWheelCallback(GLFWwindow *window, double offset_x, double offset_y)
{
    Renderer* instance = Renderer::instance_;
    if (instance->input_processor_)
        instance->input_processor_->onMouseWheel(offset_x, offset_y);
}
