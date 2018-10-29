#include "sh_renderer.h"

using namespace shr;
using namespace std;
using namespace Eigen;


// T Möller, "Fast, Minimum Storage Ray Triangle Intersection", 1997
bool Ray::isIntersectsTriangle(const vector<Vector3f> &vertices) const
{
    if (vertices.size() != 3)   return false;

    Vector3f a = vertices[0], b = vertices[1], c = vertices[2];
    Vector3f E1 = b - a, E2 = c - a;
    Vector3f P = dir.cross(E2);
    float det = E1.dot(P);

    Vector3f T;
    if (det > 0)
        T = src - a;
    else{
        T = a - src;
        det = -det;
    }

    if (det < EPSILON) return false;

    float u = T.dot(P);
    if (u <= 0 || u >= det)   return false;

    Vector3f Q = T.cross(E1);
    float v = dir.dot(Q);
    if (v <= 0 || u+v >= det) return false;

    float t = E2.dot(Q);
    if (t <= 0) return false;

    return true;
}


Matrix4f shr::lookAt(Vector3f const & eye, Vector3f const & center, Vector3f const & up)
{
    Vector3f f = (center - eye).normalized();
    Vector3f u = up.normalized();
    Vector3f s = f.cross(u).normalized();
    u = s.cross(f);

    Matrix4f res;
    res <<  s.x(),s.y(),s.z(),-s.dot(eye),
            u.x(),u.y(),u.z(),-u.dot(eye),
            -f.x(),-f.y(),-f.z(),f.dot(eye),
            0,0,0,1;

    return res;
}

Matrix4f shr::perspective(float fovy, float aspect, float zNear, float zFar)
{
    assert(aspect > 0);
    assert(zFar > zNear);

    float radf = fovy / 180.0f * PI;

    float tanHalfFovy = tan(radf / 2.0f);
    Matrix4f res = Matrix4f::Zero();
    res(0,0) = 1.0f / (aspect * tanHalfFovy);
    res(1,1) = 1.0f / (tanHalfFovy);
    res(2,2) = - (zFar + zNear) / (zFar - zNear);
    res(3,2) = - 1.0f;
    res(2,3) = - (2.0f * zFar * zNear) / (zFar - zNear);
    return res;
}
