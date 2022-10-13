#ifndef _meshs_h
#define _meshs_h

#include <igl/solid_angle.h>

class Meshs {
public:
    Meshs(Eigen::MatrixXd& v, Eigen::MatrixXi& f) {
        assert(f.cols() == 3 && "Only support 3D data. ");
        V = v;
        F = f;
    }
    double calc_winding_value(Eigen::VectorXd& p);

private:
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
};

#endif