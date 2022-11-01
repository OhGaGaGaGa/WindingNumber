#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>
#include <igl/AABB.h>
#include <igl/per_face_normals.h>
#include <igl/cross.h>
#include "Constants.h"

struct OcTreeNode {
    std::array<double, 3> _min_axis;
    std::array<double, 3> _max_axis;
    std::array<OcTreeNode*, 8> _child;
    std::vector<int> face;
    int _depth;
    double _max_dis {0};

    Eigen::Vector3d center;
    Eigen::Vector3d normal;
    double aera{};

    OcTreeNode(std::array<double, 3> min_axis, std::array<double, 3> max_axis, int depth) : 
        _min_axis(min_axis), _max_axis(max_axis),  _depth(depth), _child {nullptr} {
            for (int i = 0; i < 3; i++)
                _max_dis = std::max(_max_dis, max_axis[i] - min_axis[i]);
            if (depth < OCTREE_MAX_DEPTH) {
                std::array<double, 3> mid_axis {(min_axis[0] + max_axis[0]) / 2, (min_axis[1] + max_axis[1]) / 2, (min_axis[2] + max_axis[2]) / 2};
                _child[0] = new OcTreeNode({min_axis[0], min_axis[1], min_axis[2]}, {mid_axis[0], mid_axis[1], mid_axis[2]}, depth + 1);
                _child[1] = new OcTreeNode({mid_axis[0], min_axis[1], min_axis[2]}, {max_axis[0], mid_axis[1], mid_axis[2]}, depth + 1);
                _child[2] = new OcTreeNode({mid_axis[0], mid_axis[1], min_axis[2]}, {max_axis[0], max_axis[1], mid_axis[2]}, depth + 1);
                _child[3] = new OcTreeNode({min_axis[0], mid_axis[1], min_axis[2]}, {mid_axis[0], max_axis[1], mid_axis[2]}, depth + 1);

                _child[4] = new OcTreeNode({min_axis[0], min_axis[1], mid_axis[2]}, {mid_axis[0], mid_axis[1], max_axis[2]}, depth + 1);
                _child[5] = new OcTreeNode({mid_axis[0], min_axis[1], mid_axis[2]}, {max_axis[0], mid_axis[1], max_axis[2]}, depth + 1);
                _child[6] = new OcTreeNode({mid_axis[0], mid_axis[1], mid_axis[2]}, {max_axis[0], max_axis[1], max_axis[2]}, depth + 1);
                _child[7] = new OcTreeNode({min_axis[0], mid_axis[1], mid_axis[2]}, {mid_axis[0], max_axis[1], max_axis[2]}, depth + 1);
            }
    }

    bool inside(const Eigen::Vector3d& axis) {
        bool ret = true;
        for (int i = 0; i < 2; i++) {
            if (axis[i] < _min_axis[i] - EPS || _max_axis[i] + EPS < axis[i])
                ret = false;
        }
        return ret;
    }

    double winding_number(const Eigen::Vector3d& q) {
        auto a = normal;
        auto b = center - q;
        return (a(0) * b(0) + a(1) * b(1) + a(2) * b(2)) / pow(b.norm(), 3);
    }
};

class Meshs {
public:
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f) :_vertex(v), _mesh(f), 
        _min_axis {v.col(0).minCoeff(), v.col(1).minCoeff(), v.col(2).minCoeff()}, 
        _max_axis {v.col(0).maxCoeff(), v.col(1).maxCoeff(), v.col(2).maxCoeff()},
        _root(new OcTreeNode(_min_axis, _max_axis, 0)) {
            construct();
    }
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f, Eigen::MatrixXi& tet) : _vertex(v), _mesh(f), _tet(tet), 
        _min_axis {v.col(0).minCoeff(), v.col(1).minCoeff(), v.col(2).minCoeff()}, 
        _max_axis {v.col(0).maxCoeff(), v.col(1).maxCoeff(), v.col(2).maxCoeff()},
        _root(new OcTreeNode(_min_axis, _max_axis, 0)) {
            construct();
    }
    double calc_winding_value(const Eigen::Vector3d& p);
    double calc_winding_value_using_octree(const Eigen::Vector3d& p);

private:
    Eigen::MatrixXd _vertex;
    Eigen::MatrixXi _mesh;
    Eigen::MatrixXi _tet;
    Eigen::MatrixXd _face_normal;
    Eigen::VectorXd _aera;

    const std::array<double, 3> _min_axis;
    const std::array<double, 3> _max_axis;
    OcTreeNode* _root;
    void construct() {
        assert(_mesh.cols() == 3 && "Only support 3D data. ");
        igl::per_face_normals(_vertex, _mesh, _face_normal);
        for (auto i = 0; i < _mesh.rows(); i++)
            _face_normal.row(i).normalize();
        assert(_face_normal.row(0).norm() == 1 && "Norm Fail! ");
        _aera.resize(_mesh.rows());
        for (auto i = 0; i < _mesh.rows(); i++)
            _aera(i) = calc_aera(_mesh.row(i));
        for (auto i = 0; i < _mesh.rows(); i++) 
            insert(_root, i);
        init_octree(_root);
    }

    bool insert(OcTreeNode* node, int mesh_id);
    void init_aabb_tree();
    void init_octree(OcTreeNode* node);
    double calc_solid_angle(int mesh_id, const Eigen::Vector3d& p);
    double calc_winding_number(const Eigen::Vector3d& q, OcTreeNode* node);

    inline double calc_aera(const Eigen::Vector3i& triangle) {
        auto a = _vertex.row(triangle(0));
        auto b = _vertex.row(triangle(1));
        auto c = _vertex.row(triangle(2));

        Eigen::RowVector3d ab = b - a, ac = c - a, normal;
        igl::cross(ab, ac, normal);
        return normal.norm() / 2;
    }
    inline Eigen::Vector3d get_center(const Eigen::Vector3i& triangle) {
        auto a = _vertex.row(triangle(0));
        auto b = _vertex.row(triangle(1));
        auto c = _vertex.row(triangle(2));

        return (a + b + c) / 3;
    }
};

#endif