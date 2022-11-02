#include "Meshs.h"
#include "Constants.h"
#include <iostream>

double Meshs::calc_solid_angle(int mesh_id, const Eigen::Vector3d& p) {
    double solid_angle = igl::solid_angle(_vertex.row(_mesh(mesh_id, 0)), _vertex.row(_mesh(mesh_id, 1)), _vertex.row(_mesh(mesh_id, 2)), p);
    if (solid_angle > -0.5 - EPS && solid_angle < -0.5 + EPS || solid_angle > 0.5 - EPS && solid_angle < 0.5 + EPS)
        solid_angle = 0;
    return solid_angle;
}

double Meshs::calc_winding_value(const Eigen::Vector3d& p) {
    double tot_w = 0;
    for (int f = 0; f < _mesh.rows(); f++)
        tot_w += calc_solid_angle(f, p);
    return tot_w;
}

double Meshs::calc_winding_value_using_octree(const Eigen::Vector3d& p) {
    return calc_winding_number(p, _root);
}

double Meshs::calc_winding_number(const Eigen::Vector3d& q, OcTreeNode* node) {
    assert(node && "node cannot be nullptr");
    if ((q - node->center).norm() > ACCURACY * node->_max_dis)
        return node->winding_number(q);
    else {
        double val = 0;
        for (auto ch : node->_child)
            if (ch) val += calc_winding_number(q, ch);
        for (auto mesh_id : node->face) 
                val += calc_solid_angle(mesh_id, q);
        return val;
    }
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

void Meshs::init_octree(OcTreeNode* node) {
    if (!node) return;
    double sum_aera = 0;
    assert(node->normal.norm() == 0 && "Length of normal should be 0. ");
    assert(node->center.norm() == 0 && "Center should be (0, 0, 0). ");
    if (node->child_count) {
        for (auto& ch : node->_child) {
            init_octree(ch);
        }
        for (auto& ch : node->_child) {
            if (-EPS < ch->aera && ch->aera < EPS) {
                ch = nullptr; node->child_count--;
            }
            else {
                node->normal += ch->aera * ch->normal;
                node->center += ch->aera * ch->center;
                sum_aera += ch->aera; 
            }
        }
    }
    for (auto mesh_id : node->face) {
        node->normal += _aera(mesh_id) * _face_normal.row(mesh_id);
        node->center += _aera(mesh_id) * get_center(_mesh.row(mesh_id));
        sum_aera += _aera(mesh_id);
    }
    if (-EPS < sum_aera && sum_aera < EPS) {
        node->aera = 0;
        return;
    }
    node->center /= sum_aera;

    node->aera = node->normal.norm() / 2;
    node->normal.normalize();

    for (auto& ch : node->_child) {
        if (!ch) continue;
        auto a = ch->center - node->center;
        auto b = ch->normal;
        node->second_term_mat += ch->aera * outer(a, b);
    }
    for (auto mesh_id : node->face) {
        auto a = get_center(_mesh.row(mesh_id)) - node->center;
        auto b = _face_normal.row(mesh_id);
        node->second_term_mat += _aera(mesh_id) * outer(a, b);
    }
}

bool Meshs::insert(OcTreeNode* node, int mesh_id) {
    Eigen::Vector3i triangle = _mesh.row(mesh_id);
    int in = -1;
    if (node->child_count) {
        for (auto i = 0; i < 8; i++) {
            auto& ch = node->_child[i];
            if (!ch) continue;
            bool inside = true;
            for (auto vid : triangle) {
                if (!ch->inside(_vertex.row(vid)))
                    inside = false;
            }
            if (inside) {
                in = i;
                insert(ch, mesh_id);
            }
        }
        if (in == -1)
            node->face.push_back(mesh_id);
    }
    else 
        node->face.push_back(mesh_id);

    return true;
}