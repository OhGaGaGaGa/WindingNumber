#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>
#include <igl/AABB.h>
#include <igl/per_face_normals.h>
#include <igl/cross.h>
#include <cmath>

struct OcTreeNode {
    std::array<double, 3> _min_axis;
    std::array<double, 3> _max_axis;
    std::array<std::unique_ptr<OcTreeNode>, 8> _child;
    std::vector<int> face;
    int child_count{0};
    int _depth;
    double _max_dis{0};

    Eigen::Vector3d center {0, 0, 0};
    Eigen::Vector3d normal {0, 0, 0};
    double aera{0};
    Eigen::Matrix3d second_term_mat;

    OcTreeNode(std::array<double, 3> min_axis, std::array<double, 3> max_axis, int depth) : 
        _min_axis(min_axis), _max_axis(max_axis), _child {nullptr}, face{}, _depth(depth) {
            for (int i = 0; i < 3; i++)
                _max_dis = std::max(_max_dis, max_axis[i] - min_axis[i]);
            for (int i = 0; i < 3; i++) 
                second_term_mat.row(i) = (Eigen::Vector3d){0, 0, 0};
            // if (depth < OCTREE_MAX_DEPTH)
            //     generate_child();
    }

    bool point_inside(const Eigen::Vector3d& axis);
    double winding_number(const Eigen::Vector3d& q);
    void generate_child();
    void delete_empty_child();    
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
    std::unique_ptr<OcTreeNode> _root;

    void construct() {
        assert(_mesh.cols() == 3 && "Only support 3D data. ");
        igl::per_face_normals(_vertex, _mesh, _face_normal);
        for (auto i = 0; i < _mesh.rows(); i++)
            _face_normal.row(i).normalize();
        assert(_face_normal.row(0).norm() == 1 && "Norm Fail! ");
        _aera.resize(_mesh.rows());
        for (auto i = 0; i < _mesh.rows(); i++) {
            _aera(i) = calc_aera(_mesh.row(i));
        }
        for (auto i = 0; i < _mesh.rows(); i++) 
            _root->face.push_back(i);
        spread(_root);
        init_value(_root);
    }

    void spread(std::unique_ptr<OcTreeNode>& node);
    void init_aabb_tree();
    void init_value(std::unique_ptr<OcTreeNode>& node);
    double calc_solid_angle(int mesh_id, const Eigen::Vector3d& p);
    double calc_winding_number(const Eigen::Vector3d& q, std::unique_ptr<OcTreeNode>& node);

    inline double calc_aera(const Eigen::Vector3i& mesh_row) {
        auto a = _vertex.row(mesh_row(0));
        auto b = _vertex.row(mesh_row(1));
        auto c = _vertex.row(mesh_row(2));

        Eigen::RowVector3d ab = b - a, ac = c - a, normal;
        igl::cross(ab, ac, normal);
        return normal.norm() / 2;
    }
    inline Eigen::Vector3d get_center(const Eigen::Vector3i& mesh_row) {
        auto a = _vertex.row(mesh_row(0));
        auto b = _vertex.row(mesh_row(1));
        auto c = _vertex.row(mesh_row(2));
        return (a + b + c) / 3;
    }
    inline Eigen::Matrix3d outer(const Eigen::Vector3d& a, Eigen::Vector3d b) {
        assert(a.rows() == 3 && b.rows() == 3 && "Only Support Column Vector. ");
        return a * b.transpose();
    }
};

#endif