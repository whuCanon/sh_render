#include <iostream>

#include "sh_renderer.h"

using namespace std;
using namespace shr;
using namespace Eigen;


ObserverInput::ObserverInput(Vector3f camera_position, Vector3f camera_up)
:camera_position_(camera_position), camera_up_(camera_up)
{
    if (camera_position_.isZero())
        camera_position_ = Vector3f(0.01f, 0, 0);
    camera_view_ = lookAt(camera_position_, Vector3f::Zero(), camera_up_);
    model_trans_ = Matrix4f::Identity();
}


void ObserverInput::onMouseButton(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT){
        lbutton_down_ = (action == GLFW_PRESS);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT){
        rbutton_down_ = (action == GLFW_PRESS);
    }

}

void ObserverInput::onMouseMove(double x, double y)
{
    if (rbutton_down_)    // drag scene (camera move)
    {
        double dx = (x - lastx_) * 0.01;
        double dy = (lasty_ - y) * 0.01;

        Vector3f dir = camera_position_.normalized();
        Vector3f right = dir.cross(camera_up_).normalized();
        Affine3f hrot(AngleAxisf(float(dx), camera_up_));
        Affine3f vrot(AngleAxisf(float(dy), right));

        Vector3f newpos = hrot * vrot * camera_position_;
        Vector3f newdir = newpos.normalized();

        if (abs(newdir.dot(camera_up_)) < 0.99f){
            camera_position_ = newpos;
            camera_view_ = lookAt(newpos, Vector3f::Zero(), camera_up_);
        }
    }

    if (lbutton_down_)  // drag model (model move)
    {
        double dx = (x - lastx_) * 0.01;
        double dy = (lasty_ - y) * 0.01;

        Vector3f dir = camera_position_.normalized();
        Vector3f right = dir.cross(camera_up_).normalized();
        Affine3f hrot(AngleAxisf(float(dx), camera_up_));
        Affine3f vrot(AngleAxisf(float(dy), right));

        model_trans_ = vrot * hrot * model_trans_;
    }

    lastx_ = x;
    lasty_ = y;
}

void ObserverInput::onMouseWheel(double offset_x, double offset_y)
{
    double dw = 1.0 - offset_y * 0.1;
    Vector3f newpos = Scaling(static_cast<float>(dw)) * camera_position_;
    float cam_dist = newpos.norm();

    if (cam_dist < 1000 && cam_dist > 0.1f)
    {
        camera_position_ = newpos;
        camera_view_ = lookAt(newpos, Vector3f::Zero(), camera_up_);
    }
}
