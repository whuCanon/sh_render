#ifndef INPUTS_H
#define INPUTS_H

#include <Eigen/Dense>

using namespace Eigen;


namespace shr {
    class InputProcessor
    {
    public:
        InputProcessor() = default;
        virtual ~InputProcessor() = default;
        virtual void onKey(int key, int scancode, int action, int mods) {}
        virtual void onMouseButton(int button, int action, int mods) {}
        virtual void onMouseMove(double x, double y) {}
        virtual void onMouseWheel(double offset_x, double offset_y) {}
    };


    class ObserverInput : public InputProcessor
    {
    public:
        ObserverInput(Vector3f camera_position = Vector3f(1, 0, 0), Vector3f camera_up = Vector3f(0, 1, 0));

        Matrix4f getCameraView() const { return camera_view_; }
        Matrix4f getModelTransform() const { return model_trans_; }
        float getLightIntensity() const { return light_intensity_; }

        void onKey(int key, int scancode, int action, int mods) override;
        void onMouseButton(int button, int action, int mods) override;
        void onMouseMove(double x, double y) override;
        void onMouseWheel(double offset_x, double offset_y) override;

        bool flag = false;
        bool mode = false;  // false means pano_viewer is set, true means render mode is set.

    private:
        Vector3f camera_position_;
        Vector3f camera_up_;
        Matrix4f camera_view_;
        Matrix4f model_trans_;

        bool lbutton_down_ = false;
        bool rbutton_down_ = false;
        double lastx_ = 0, lasty_ = 0;
        float light_intensity_ = 0.05f;

    };
}
#endif // INPUTS_H
