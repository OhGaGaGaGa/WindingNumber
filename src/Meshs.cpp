#include "Meshs.h"

double Meshs::calc_winding_value(Eigen::VectorXd p) {
    assert(p.size() == 3 && "Only support 3D data. ");
    double tot_w = 0;
    for (int f = 0; f < F.cols(); f++) {
        tot_w += igl::solid_angle(V.row(F(f, 0)), V.row(F(f, 1)), V.row(F(f, 2)), p);
    }
    return tot_w;
}

