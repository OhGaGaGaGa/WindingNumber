#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Eigen/Core>
#include "Constants.h"

// TODO: inline
inline double random_from_to(double left, double right) {
    return left + (right - left) * rand() / RAND_MAX;
}

Eigen::MatrixXd random_points(int cnt, double l0, double r0, double l1, double r1, double l2, double r2) {
    Eigen::MatrixXd ret(cnt, 3);
    for (int i = 0; i < cnt; i++) {
        ret(i, 0) = random_from_to(l0, r0);
        ret(i, 1) = random_from_to(l1, r1);
        ret(i, 2) = random_from_to(l2, r2);
    }
    return ret;
}

Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V) {
    srand(time(nullptr));

    // TODO: highly redundant code
    double min0 = INT_MAX, max0 = INT_MIN;
    double min1 = INT_MAX, max1 = INT_MIN;
    double min2 = INT_MAX, max2 = INT_MIN;
    for (int i = 0; i < V.rows(); i++) {
        min0 = std::min(min0, V(i, 0));
        max0 = std::max(max0, V(i, 0));
        min1 = std::min(min1, V(i, 1));
        max1 = std::max(max1, V(i, 1));
        min2 = std::min(min2, V(i, 2));
        max2 = std::max(max2, V(i, 2));
    }

    return random_points(TESTSIZE,
        min0 - PADDING, max0 + PADDING,
        min1 - PADDING, max1 + PADDING,
        min2 - PADDING, max2 + PADDING
    );
}