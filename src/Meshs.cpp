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

    Eigen::MatrixXd P; // p * 3
    Eigen::VectorXd sqrD; // p * 1, smallest squared distances
    Eigen::VectorXi I; // p * 1, primitive indices corresponding to smallest distances
    Eigen::MatrixXd C; // p * 3, closest points
    tree.squared_distance(_vertex, _mesh, P, sqrD, I, C);
}

void Meshs::init_octree() {
    igl::per_face_normals(_vertex, _mesh, _face_normal);

    std::array<double, 3> min_axis {_vertex.col(0).minCoeff(), _vertex.col(1).minCoeff(), _vertex.col(2).minCoeff()};
    std::array<double, 3> max_axis {_vertex.col(0).maxCoeff(), _vertex.col(1).maxCoeff(), _vertex.col(2).maxCoeff()};

    OcTree tree(min_axis, max_axis);

    for (auto i = 0; i < _mesh.rows(); i++) {
        tree.insert(_mesh.row(i));
    }
}

bool OcTree::insert(const Eigen::Vector3i& triangle) {
    
}