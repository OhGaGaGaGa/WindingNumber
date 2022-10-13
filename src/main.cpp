#include <iostream>
#include <cstdlib>
#include <ctime>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/solid_angle.h>
#include "Constants.h"
#include "Meshs.h"
#include "VTKwriter.h"

using namespace std;

double random_from_to(double left, double right) {
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

int main() {
    srand(10);

    Eigen::MatrixXd inputV;
    Eigen::MatrixXi inputF;
    igl::readOBJ("../files/tiger-in.obj", inputV, inputF);

    std::cout << inputF.cols() << std::endl;

    for (int i = 0; i < inputF.cols(); i++)
        std::cout << inputF(0, i) << " ";
    std::cout << std::endl;

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

    cout << min0 << " " << max0 << endl;
    cout << min1 << " " << max1 << endl;
    cout << min2 << " " << max2 << endl;

    Eigen::MatrixXd testV = random_points(TestSize,
        min0 - Padding, max0 + Padding,
        min1 - Padding, max1 + Padding,
        min2 - Padding, max2 + Padding
    );

    std::array<double, TestSize> judge{ 0 };

    for (int i = 0; i < testV.rows(); i++) {
        Eigen::VectorXd tmpVec(3);
        tmpVec << testV(i, 0), testV(i, 1), testV(i, 2);
        judge[i] = meshs.calc_winding_value(tmpVec);

        std::cout << judge[i] << " ";
    }

    VTKwriter writer("../files/tiger-out.vtk", testV, judge);
    writer.write_colored_points();

    return 0;
}