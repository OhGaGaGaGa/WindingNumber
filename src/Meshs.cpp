#include "Meshs.h"
#include "Constants.h"
#include <iostream>

double Meshs::calc_winding_value(const Eigen::Vector3d& p) {
    double tot_w = 0;
    for (int f = 0; f < _mesh.rows(); f++) {
        double solid_angle = igl::solid_angle(_vertex.row(_mesh(f, 0)), _vertex.row(_mesh(f, 1)), _vertex.row(_mesh(f, 2)), p);
        if (solid_angle > -0.5 - EPS && solid_angle < -0.5 + EPS || solid_angle > 0.5 - EPS && solid_angle < 0.5 + EPS)
            solid_angle = 0;
        tot_w += solid_angle;
    }
    return tot_w;
}

void Meshs::init_aabb_tree() {
    igl::AABB<Eigen::MatrixXd, 3> tree;
    tree.init(_vertex, _mesh);
    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;
    Eigen::Vector3d P;
    tree.squared_distance(_vertex, _mesh, P, sqrD, I, C);
}