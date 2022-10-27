#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>
#include <igl/AABB.h>

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
    void init_aabb_tree();
};

#endif