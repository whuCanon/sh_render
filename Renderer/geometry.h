#ifndef SH_GEOMETRY_H
#define SH_GEOMETRY_H

#include <cmath>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <sh/spherical_harmonics.h>

#include "common.h"

using namespace std;
using namespace Eigen;


namespace shr
{
    struct Vertex
    {
        Vector3f position;
        Vector3f normal;
        Vector2f texcoords;
        float render_coeffs[SH_NUM] = {0.0f};
    };


    class BoundaryBox
    {
    public:
        BoundaryBox() = default;
        BoundaryBox(float a, float b, float c, float d, float e, float f) :
            xmin_(a), xmax_(b), ymin_(c), ymax_(d), zmin_(e), zmax_(f) {}

        float xmin() const { return xmin_; }
        float xmax() const { return xmax_; }
        float ymin() const { return ymin_; }
        float ymax() const { return ymax_; }
        float zmin() const { return zmin_; }
        float zmax() const { return zmax_; }
        float length() { return xmax_ - xmin_; }
        float height() { return ymax_ - ymin_; }
        float width()  { return zmax_ - zmin_; }

        void setYmin(const float &y) { ymin_ = y; }

        bool isVertexInBox(Vector3f &v);
        void updateBound(Vector3f &v);

    private:
        float xmin_ = 0;
        float xmax_ = 0;
        float ymin_ = 0;
        float ymax_ = 0;
        float zmin_ = 0;
        float zmax_ = 0;
    };


    class Ray
    {
    public:
        Ray() = default;
        Ray(Vector3f s, Vector3f d) : src(s) { dir = d.normalized(); }

        void setRay(Vector3f s, Vector3f d) { src = s;  dir = d.normalized(); }

        bool isIntersectsTriangle(const vector<Vector3f> &vertices) const;

        Vector3f src;			//Source of ray
        Vector3f dir;			//Direction (normalised)
    };


    Matrix4f lookAt(Vector3f const & eye, Vector3f const & center, Vector3f const & up);
    Matrix4f perspective(float fovy, float aspect, float zNear, float zFar);

    void lightRotate(const Matrix4f &rotation, float coeffs[SH_NUM], float (*result)[SH_NUM]);
}

#endif // SH_GEOMETRY_H
