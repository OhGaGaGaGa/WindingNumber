#include "Meshs.h"
#include "Constants.h"
#include <iostream>

bool OcTreeNode::point_inside(const Eigen::Vector3d& axis) {
    bool ret = true;
    for (int i = 0; i < 3; i++) {
        if (axis[i] < _min_axis[i] - EPS || _max_axis[i] + EPS < axis[i])
            ret = false;
    }
    return ret;
}

double OcTreeNode::winding_number(const Eigen::Vector3d& q) {
    auto a = normal;
    auto b = center - q;
    auto first_term = (a(0) * b(0) + a(1) * b(1) + a(2) * b(2)) / pow(b.norm(), 3);

    Eigen::Matrix3d Hesse;
    Hesse << b.squaredNorm()-3*b(0)*b(0), -3 * b(0) * b(1),            -3 * b(0) * b(2), 
             -3 * b(1) * b(0),            b.squaredNorm()-3*b(1)*b(1), -3 * b(1) * b(2),
             -3 * b(2) * b(0),            -3 * b(2) * b(1),             b.squaredNorm()-3*b(2)*b(2);
    Hesse /= b.squaredNorm() * b.squaredNorm() * b.norm();
    double second_term = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            second_term += second_term_mat(i, j) * Hesse(i, j);
    if (first_term > EPS && abs(first_term) > abs(second_term) || first_term < -EPS && abs(first_term) > abs(second_term))
        return first_term + second_term;
    return first_term;
}

void OcTreeNode::generate_child() {
    std::array<double, 3> mid {(_min_axis[0] + _max_axis[0]) / 2, (_min_axis[1] + _max_axis[1]) / 2, (_min_axis[2] + _max_axis[2]) / 2};
    auto& min = _min_axis;
    auto& max = _max_axis;
    _child[0] = new OcTreeNode({min[0], min[1], min[2]}, {mid[0], mid[1], mid[2]}, _depth + 1);
    _child[1] = new OcTreeNode({mid[0], min[1], min[2]}, {max[0], mid[1], mid[2]}, _depth + 1);
    _child[2] = new OcTreeNode({mid[0], mid[1], min[2]}, {max[0], max[1], mid[2]}, _depth + 1);
    _child[3] = new OcTreeNode({min[0], mid[1], min[2]}, {mid[0], max[1], mid[2]}, _depth + 1);

    _child[4] = new OcTreeNode({min[0], min[1], mid[2]}, {mid[0], mid[1], max[2]}, _depth + 1);
    _child[5] = new OcTreeNode({mid[0], min[1], mid[2]}, {max[0], mid[1], max[2]}, _depth + 1);
    _child[6] = new OcTreeNode({mid[0], mid[1], mid[2]}, {max[0], max[1], max[2]}, _depth + 1);
    _child[7] = new OcTreeNode({min[0], mid[1], mid[2]}, {mid[0], max[1], max[2]}, _depth + 1);

    child_count = 8;
}

void OcTreeNode::delete_empty_child() {}

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
                delete ch;
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

void Meshs::spread(OcTreeNode* node) {
    if (node->face.size() <= 2)
        return;
    node->generate_child();
    auto vec = node->face;
    node->face.clear();
    for (auto mesh_id : vec) {
        Eigen::Vector3i triangle = _mesh.row(mesh_id);
        int in = -1;
        for (auto i = 0; i < 8; i++) {
            auto& ch = node->_child[i];
            bool inside = true;
            for (auto vid : triangle) {
                if (!ch->point_inside(_vertex.row(vid)))
                    inside = false;
            }
            if (inside) {
                in = i;
                ch->face.push_back(mesh_id);
            }
        }
        if (in == -1)
            node->face.push_back(mesh_id);
    }
    for (auto& ch : node->_child) {
        spread(ch);
    }
    node->delete_empty_child();
}