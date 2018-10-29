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
}

#endif // SH_GEOMETRY_H
