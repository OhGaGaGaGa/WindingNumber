#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>
#include <igl/AABB.h>
#include <igl/per_face_normals.h>
#include "Constants.h"

class Meshs {
public:
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f) :_vertex(v), _mesh(f) {
        assert(f.cols() == 3 && "Only support 3D data. ");
    }
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f, Eigen::MatrixXd& tet) :_vertex(v), _mesh(f), _tet(tet) {
        assert(f.cols() == 3 && "Only support 3D data. ");
    }
    double calc_winding_value(const Eigen::Vector3d& p);

private:
    Eigen::MatrixXd _vertex;
    Eigen::MatrixXi _mesh;
    Eigen::MatrixXi _tet;
    Eigen::MatrixXd _face_normal;
    void init_aabb_tree();
    void init_octree();
};

struct OcTreeNode {
    std::array<double, 3> _min_axis;
    std::array<double, 3> _max_axis;
    std::array<OcTreeNode*, 8> _child;
    int _depth;

    OcTreeNode(std::array<double, 3> min_axis, std::array<double, 3> max_axis, int depth) : 
        _min_axis(min_axis), _max_axis(max_axis),  _depth(depth), _child {nullptr} {
            if (depth < OCTREE_MAX_DEPTH) {
                std::array<double, 3> mid_axis {(min_axis[0] + max_axis[0]) / 2, (min_axis[1] + max_axis[1]) / 2, (min_axis[2] + max_axis[2]) / 2};
                _child[0] = new OcTreeNode({min_axis[0], min_axis[1], min_axis[2]}, {mid_axis[0], mid_axis[1], mid_axis[2]}, depth + 1);
                _child[1] = new OcTreeNode({mid_axis[0], min_axis[1], min_axis[2]}, {max_axis[0], mid_axis[1], mid_axis[2]}, depth + 1);
                
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
};

class OcTree {
public:
    OcTree(const std::array<double, 3>& min_axis, const std::array<double, 3>& max_axis) : 
        _min_axis(min_axis), _max_axis(max_axis), _root(new OcTreeNode(min_axis, max_axis, 0)) {}
    bool insert(const Eigen::Vector3i& triangle);
private:
    const std::array<double, 3> _min_axis;
    const std::array<double, 3> _max_axis;
    OcTreeNode* _root;
};

#endif