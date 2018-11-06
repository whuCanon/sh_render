#ifndef COMMON_H
#define COMMON_H

#include <cmath>

const float PI = float(M_PI);
const float EPSILON = 1e-5f;

const int PLANE_VNUM = 10000;     // vertices number of plane, a square number is recomended

const int L = 4;                // spherical hamonics order
const int SH_NUM = (L+1)*(L+1); // number of SH coefficients
const int SAMPLE_NUM = 10000;     // number of Monte Carlo sampling, a square number is recomended

// some test parameter
const int SCALE = 1;

#endif // COMMON_H
