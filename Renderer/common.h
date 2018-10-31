#ifndef COMMON_H
#define COMMON_H

#include <cmath>

const float PI = float(M_PI);
const float EPSILON = 1e-5f;

const int L = 4;                // spherical hamonics order
const int SH_NUM = (L+1)*(L+1); // number of SH coefficients
const int SAMPLE_NUM = 100;     // number of Monte Carlo sampling, a square number is recomended

#endif // COMMON_H
