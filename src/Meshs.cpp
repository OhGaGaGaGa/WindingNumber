#include "Meshs.h"

#include <iostream>

double Meshs::calc_winding_value(Eigen::Vector3d& p) {
    double tot_w = 0;
    std::cerr << "Query Point: " << p(0) << " " << p(1) << " " << p(2) << std::endl;
    for (int f = 0; f < _mesh.rows(); f++) {
        tot_w += igl::solid_angle(_vertex.row(_mesh(f, 0)), _vertex.row(_mesh(f, 1)), _vertex.row(_mesh(f, 2)), p);
        std::cerr << igl::solid_angle(_vertex.row(_mesh(f, 0)), _vertex.row(_mesh(f, 1)), _vertex.row(_mesh(f, 2)), p) << " ";
    }
    std::cerr << std::endl;
    return tot_w;
}

// bool Meshs::inside(double p, double x1, double x2, double x3) {
//     double min_x = std::min(x1, std::min(x2, x3));
//     double max_x = std::max(x1, std::max(x2, x3));
//     return p >= min_x && p <= max_x;
// }

// double Meshs::calc_winding_value(Eigen::Vector3d& p) {
//     double ret = 0;
//     for (int f = 0; f < _mesh.cols(); f++) {
//         if (inside(p(0), (_mesh(f, 0), 0), _vertex(_mesh(f, 1), 0), _vertex(_mesh(f, 2), 0)) &&
//             inside(p(1), _vertex(_mesh(f, 0), 1), _vertex(_mesh(f, 1), 1), _vertex(_mesh(f, 2), 1)))
//             ret += igl::solid_angle(_vertex.row(_mesh(f, 0)), _vertex.row(_mesh(f, 1)), _vertex.row(_mesh(f, 2)), p) > 0 ? 1 : -1;
//     }
//     return ret;
// }
