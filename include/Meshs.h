#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>

class Meshs {
public:
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f) :_V(v), _F(f) {
        assert(f.cols() == 3 && "Only support 3D data. ");
    }
    double calc_winding_value(Eigen::Vector3d& p);

private:
    Eigen::MatrixXd _V;
    Eigen::MatrixXi _F;

    bool inside(double p, double x1, double x2, double x3);
};

#endif