#include <math.h>
#include "sh_sampler.h"


const double PI = double(M_PI);

template class HarmonicIntegral<4>;
template class HarmonicIntegral<9>;
template class HarmonicIntegral<16>;
template class HarmonicIntegral<25>;

/* reference: https://en.wikipedia.org/wiki/Table_of_spherical_harmonics.
   t : theta, [0, PI],  angel between ray and z-axis;
   p : phi,   [0, 2PI), angel between ray and x-axis in x-y plane.
 */
template<>
void HarmonicBasis<4>(double basis[4], double t, double p)
{
    basis[0] = 1.0 / 2*sqrt(1/PI);              // l=0, m= 0

    basis[1] = sqrt(3/(4*PI)) * sin(t)*sin(p);  // l=1, m=-1
    basis[2] = sqrt(3/(4*PI)) * cos(t);         // l=1, m= 0
    basis[3] = sqrt(3/(4*PI)) * sin(t)*cos(p);  // l=1, m= 1
}

template<>
void HarmonicBasis<9>(double basis[9], double t, double p)
{
    HarmonicBasis<4>(basis, t, p);
    basis[4] = 1.0 / 4 * sqrt(15/PI) * sin(t)*sin(t)*sin(2*p);  // l=2, m=-2
    basis[5] = 1.0 / 2 * sqrt(15/PI) * sin(t)*cos(t)*sin(p);    // l=2, m=-1
    basis[6] = 1.0 / 4 * sqrt(5/PI)  * (3*cos(t)*cos(t) - 1);   // l=2, m= 0
    basis[7] = 1.0 / 2 * sqrt(15/PI) * sin(t)*cos(t)*cos(p);    // l=2, m= 1
    basis[8] = 1.0 / 4 * sqrt(15/PI) * sin(t)*sin(t)*cos(2*p);  // l=2, m= 2
}

template<>
void HarmonicBasis<16>(double basis[16], double t, double p)
{
    HarmonicBasis<9>(basis, t, p);
    basis[9]  = 1.0 / 4 * sqrt(35/(2*PI)) * sin(t)*sin(t)*sin(t)*sin(3*p);      // l=3, m=-3
    basis[10] = 1.0 / 4 * sqrt(105/PI)    * cos(t)*sin(t)*sin(t)*sin(2*p);      // l=3, m=-2
    basis[11] = 1.0 / 4 * sqrt(21/(2*PI)) * (5*cos(t)*cos(t)-1)*sin(t)*sin(p);  // l=3, m=-1
    basis[12] = 1.0 / 4 * sqrt(7/PI)      * (5*cos(t)*cos(t)*cos(t)-3*cos(t));  // l=3, m= 0
    basis[13] = 1.0 / 4 * sqrt(21/(2*PI)) * (5*cos(t)*cos(t)-1)*sin(t)*cos(p);  // l=3, m= 1
    basis[14] = 1.0 / 4 * sqrt(105/PI)    * cos(t)*sin(t)*sin(t)*cos(2*p);;     // l=3, m= 2
    basis[15] = 1.0 / 4 * sqrt(35/(2*PI)) * sin(t)*sin(t)*sin(t)*cos(3*p);      // l=3, m= 3
}

template<>
void HarmonicBasis<25>(double basis[25], double t, double p)
{
    HarmonicBasis<16>(basis, t, p);
    basis[16] = 3.0 / 16 * sqrt(35/PI)     * sin(t)*sin(t)*sin(t)*sin(t)*sin(4*p);                  // l=4, m=-4
    basis[17] = 3.0 / 4  * sqrt(35/(2*PI)) * cos(t)*sin(t)*sin(t)*sin(t)*sin(3*p);                  // l=4, m=-3
    basis[18] = 3.0 / 8  * sqrt(5/PI)      * (7*cos(t)*cos(t)-1)*sin(t)*sin(t)*sin(2*p);            // l=4, m=-2
    basis[19] = 3.0 / 4  * sqrt(5/(2*PI))  * (7*cos(t)*cos(t)*cos(t)-3*cos(t))*sin(t)*sin(p);       // l=4, m=-1
    basis[20] = 3.0 / 16 * sqrt(1/PI)      * (35*cos(t)*cos(t)*cos(t)*cos(t)-30*cos(t)*cos(t)+3);   // l=4, m= 0
    basis[21] = 3.0 / 4  * sqrt(5/(2*PI))  * (7*cos(t)*cos(t)*cos(t)-3*cos(t))*sin(t)*cos(p);       // l=4, m= 1
    basis[22] = 3.0 / 8  * sqrt(5/PI)      * (7*cos(t)*cos(t)-1)*sin(t)*sin(t)*cos(2*p);            // l=4, m= 2
    basis[23] = 3.0 / 4  * sqrt(35/(2*PI)) * cos(t)*sin(t)*sin(t)*sin(t)*cos(3*p);                  // l=4, m= 3
    basis[24] = 3.0 / 16 * sqrt(35/PI)     * sin(t)*sin(t)*sin(t)*sin(t)*cos(4*p);                  // l=4, m= 4
}


template<int COEF_NUM>
void HarmonicIntegral<COEF_NUM>::solvePanoImage(const string image_path)
{
    for (int i = 0; i < COEF_NUM; i++)
        coef_[i] = 0;

    Mat gray_pano_img = imread(image_path, IMREAD_GRAYSCALE | IMREAD_ANYDEPTH);
    resize(gray_pano_img, gray_pano_img, pano_size_);
    Matrix<uchar, Dynamic, Dynamic> gray_pano;
    cv2eigen(gray_pano_img, gray_pano);

    srand(static_cast<unsigned>(time(nullptr)));
    for (int c = 0; c < count_; c++)
    {
        double u = rand() / double(RAND_MAX);
        double v = rand() / double(RAND_MAX);
        array<int, 2> coords = uv2coords(u, v);
        array<double, 2> polar = uv2polar(u, v);

        double basis[COEF_NUM];
        HarmonicBasis<COEF_NUM>(basis, polar[0], polar[1]);

        for (int k = 0; k < COEF_NUM; k++)
            coef_[k] += gray_pano(coords[0], coords[1]) * basis[k];
    }

    for (auto& c : coef_){
        c = c * 4 * PI / count_;
        if(gray_pano_img.type() == 0)
            c /= 2550;
    }
}


template<int COEF_NUM>
void HarmonicIntegral<COEF_NUM>::solveRawImage(const string shading_path, const string normal_path)
{
    Mat normal_cv  = imread(normal_path);
    Mat shading_cv = imread(shading_path, IMREAD_GRAYSCALE);

    int rows_raw = normal_cv.rows, cols_raw = normal_cv.cols;
    //float scale = cols_raw / rows_raw;
    //int cols = 640; int rows = static_cast<int>(cols / scale);
    int cols = cols_raw, rows = rows_raw;
    resize(normal_cv, normal_cv, Size(cols, rows));
    resize(shading_cv, shading_cv, Size(cols, rows));

    A_ = MatrixXf::Zero(rows*cols, COEF_NUM);
    b_ = VectorXf::Zero(rows*cols);

    int count = 100000;
    int sqrt_c = sqrt(static_cast<double>(count));
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < rows; i++)
    {
        Vec3b *ptr_n = normal_cv.ptr<Vec3b>(i);
        uchar *ptr_s = shading_cv.ptr<uchar>(i);
        for (int j = 0; j < cols; j++)
        {
            for (int c = 0; c < count; c++)
            {
                double phi = 2*PI * (rand() / double(RAND_MAX));
                double theta = acos(1 - 2*(rand()/double(RAND_MAX)));
//                double phi = 2*PI * ((0.5+c/sqrt_c) / sqrt_c);
//                double theta = acos(1 - 2 * ((0.5+i%sqrt_c) / sqrt_c));
                double rx = sin(theta) * cos(phi);
                double ry = sin(theta) * sin(phi);
                double rz = cos(theta);
                //double x = ptr_n[j][2], y = ptr_n[j][1], z = ptr_n[j][0];
                double y = ptr_n[j][2], z = ptr_n[j][1], x = ptr_n[j][0];   // camera axis to world axis
                double dot = rx*x + ry*y + rz*z;
                if (dot < 0)    continue;

                double r = sqrt(x*x + y*y + z*z);
                double basis[COEF_NUM];
                HarmonicBasis<COEF_NUM>(basis, theta, phi);
                //HarmonicBasis<COEF_NUM>(basis, acos(z/r), atan(y/x));
                for (int k = 0; k < COEF_NUM; k++)
                    A_(i*cols + j, k) += basis[k] * dot / r;    // basis * cos()
            }

            b_(i*cols + j) = ptr_s[j];
        }
    }
    A_ /= count / PI;

    VectorXf coef = A_.colPivHouseholderQr().solve(b_);
    for ( int i = 0; i < COEF_NUM; i++)
        coef_[i] = coef(i);
}


template<int COEF_NUM>
void HarmonicIntegral<COEF_NUM>::writeSHPanoImage(const string image_path)
{
    const int w = pano_size_.width, h = pano_size_.height;
    MatrixXd SH_mat = MatrixXd::Zero(h, w);

    for (int i = 0; i < w; i++)
    {
        double phi = (1 - 1.0*i/w) * 2*PI;
        for (int j = 0; j < h; j++)
        {
            double theta = j * PI/h;
            double basis[COEF_NUM];
            HarmonicBasis<COEF_NUM>(basis, theta, phi);

            double temp = 0;
            for (int k = 0; k < COEF_NUM; k++)
                temp += coef_[k] * basis[k];
            SH_mat(j, i) = (temp < 0) ? 0 : temp;
        }
    }

    double max = SH_mat.maxCoeff(), min = SH_mat.minCoeff();
    SH_mat = 255 * (SH_mat.array() - min) / (max - min);

    Mat output_img(pano_size_, CV_8UC1);
    eigen2cv(SH_mat, output_img);
    imwrite(image_path, output_img);
}


 // reference: http://mathworld.wolfram.com/SpherePointPicking.html
template<int COEF_NUM>
array<int, 2> HarmonicIntegral<COEF_NUM>::uv2coords(double u, double v)
{
    int col = pano_size_.width  * u;
    int row = pano_size_.height * (1 - acos(2*v - 1) / PI);

    array<int, 2> result = {row, col};
    return result;
}

template<int COEF_NUM>
array<double, 2> HarmonicIntegral<COEF_NUM>::uv2polar(double u, double v)
{
    double phi   = u * 2*PI;
    double theta = PI - acos(2*v - 1);

    array<double, 2> result = {theta, phi};
    return  result;
}

