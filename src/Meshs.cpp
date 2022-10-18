#include "Meshs.h"

// double Meshs::calc_winding_value(Eigen::VectorXd& p) {
//     assert(p.size() == 3 && "Only support 3D data. ");
//     double tot_w = 0;
//     for (int f = 0; f < _F.cols(); f++) {
//         tot_w += igl::solid_angle(_V.row(_F(f, 0)), _V.row(_F(f, 1)), _V.row(_F(f, 2)), p);
//     }
//     return tot_w;
// }

bool Meshs::inside(double p, double x1, double x2, double x3) {
    double min_x = std::min(x1, std::min(x2, x3));
    double max_x = std::max(x1, std::max(x2, x3));
    return p >= min_x && p <= max_x;
}

double Meshs::calc_winding_value(Eigen::Vector3d& p) {
    assert(p.size() == 3 && "Only support 3D data. ");
    double ret = 0;
    for (int f = 0; f < _F.cols(); f++) {
        if (inside(p(0), (_F(f, 0), 0), _V(_F(f, 1), 0), _V(_F(f, 2), 0)) &&
            inside(p(1), _V(_F(f, 0), 1), _V(_F(f, 1), 1), _V(_F(f, 2), 1)))
            ret += igl::solid_angle(_V.row(_F(f, 0)), _V.row(_F(f, 1)), _V.row(_F(f, 2)), p) > 0 ? 1 : -1;
    }
    return ret;
}
