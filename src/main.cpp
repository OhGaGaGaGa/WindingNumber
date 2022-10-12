#include <iostream>
#include <cstdlib>
#include <ctime>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/solid_angle.h>
#include "Meshs.h"

using namespace std;

double randomFromTo(int left, int right) {
    return left + (right - left) * rand() / RAND_MAX;
}

Eigen::MatrixXd randomPoints(int cnt, double l0, double r0, double l1, double r1, double l2, double r2) {
    Eigen::MatrixXd ret(cnt, 3);
    for (int i = 0; i < cnt; i++) {
        ret(i, 0) = randomFromTo(l0, r0);
        ret(i, 1) = randomFromTo(l1, r1);
        ret(i, 2) = randomFromTo(l2, r2);
    }
    return ret;
}

int main() {
    srand(time(0));

    Eigen::MatrixXd inputV;
    Eigen::MatrixXi inputF;
    igl::readOBJ("./files/tiger-in.obj", inputV, inputF);

    Meshs meshs(inputV, inputF);

    double min0 = INT_MAX, max0 = INT_MIN;
    double min1 = INT_MAX, max1 = INT_MIN;
    double min2 = INT_MAX, max2 = INT_MIN;
    for (int i = 0; i < inputV.rows(); i++) {
        min0 = std::min(min0, inputV(i, 0));
        max0 = std::max(max0, inputV(i, 0));
        min1 = std::min(min1, inputV(i, 1));
        max1 = std::max(max1, inputV(i, 1));
        min2 = std::min(min2, inputV(i, 2));
        max2 = std::max(max2, inputV(i, 2));
    }

    const double Padding = 10;
    const int TestSize = 100;

    Eigen::MatrixXd testV = randomPoints(TestSize,
        min0 - Padding, max0 + Padding,
        min1 - Padding, max1 + Padding,
        min2 - Padding, min2 + Padding
    );

    double judge[TestSize];

    for (int i = 0; i < testV.rows(); i++) {
        Eigen::VectorXd tmpVec(3);
        tmpVec << testV(i, 0), testV(i, 1), testV(i, 2);
        judge[i] = meshs.calc_winding_value(tmpVec);
    }


    return 0;
}