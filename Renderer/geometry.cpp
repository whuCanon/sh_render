#include "framework.h"

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


bool BoundaryBox::isVertexInBox(Vector3f &v)
{
    if (v.x() < xmin_)      return false;
    else if (v.x() > xmax_) return false;

    if (v.y() < ymin_)      return false;
    else if (v.y() > ymax_) return false;

    if (v.z() < zmin_)      return false;
    else if (v.z() > zmax_) return false;

    return true;
}

void BoundaryBox::updateBound(Vector3f &v)
{
    if (v.x() < xmin_)      xmin_ = v.x();
    else if (v.x() > xmax_) xmax_ = v.x();

    if (v.y() < ymin_)      ymin_ = v.y();
    else if (v.y() > ymax_) ymax_ = v.y();

    if (v.z() < zmin_)      zmin_ = v.z();
    else if (v.z() > zmax_) zmax_ = v.z();
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


void shr::lightRotate(const Matrix4f &rotation, float coeffs[SH_NUM], float (&result)[SH_NUM])
{
    Matrix3f temp_trans;
    temp_trans << rotation(0,0), rotation(0,1), rotation(0,2),
                  rotation(1,0), rotation(1,1), rotation(1,2),
                  rotation(2,0), rotation(2,1), rotation(2,2);

    Quaterniond light_trans(temp_trans.cast<double>());
    unique_ptr<sh::Rotation> light_rotation = sh::Rotation::Create(L, light_trans.normalized());

    vector<float> temp_coeffs, temp_result;
    for (int i = 0; i < SH_NUM; i++)
        temp_coeffs.push_back(coeffs[i]);

    light_rotation->Apply(temp_coeffs, &temp_result);

    for (int i = 0; i < SH_NUM; i++)
        result[i] = temp_result[i];
}
