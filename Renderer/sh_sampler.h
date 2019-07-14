#include <array>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>

using namespace cv;
using namespace std;
using namespace Eigen;


// normalize first
template<int COEF_NUM>
void HarmonicBasis(double basis[COEF_NUM], double t, double p);            // Spherical Harmonic basis

template <int COEF_NUM>
class HarmonicIntegral{
public:
    HarmonicIntegral();
    HarmonicIntegral(const int count) : count_(count) {}
    std::array<double, COEF_NUM> getCoefficients() { return coef_; }

    // for sampling from panorama
    void solvePanoImage(const string image_path);                           // read and sample SH_coef from panorama image
    // for estimating from single image
    void solveRawImage(const string shading_path, const string normal_path);// read and process normal and shading image, solve LMS
    void writeSHPanoImage(const string image_path);                         // write the corresponding panorama using SH_coef

private:
    const int count_;
    Size pano_size_ = Size(1024, 512);
    std::array<double, COEF_NUM> coef_;
    // for estimating from single image
    Size img_size_;
    MatrixXf A_;
    VectorXf b_;

    array<int, 2> uv2coords(double u, double v);
    array<double, 2> uv2polar(double u, double v);
};
