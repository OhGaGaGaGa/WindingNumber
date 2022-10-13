#ifndef _vtkwriter_h
#define _vtkwriter_h

#include <fstream>
#include <string>
#include <Eigen/Dense>
#include "Constants.h"
#include "Eigen/src/Core/Matrix.h"

class VTKwriter {
public:
    VTKwriter(const char* file, Eigen::MatrixXd& v, std::array<double, TestSize>& j) : filepath(file), J{ 0 } {
        assert(v.cols() == 3 && "Only support 3D data. ");
        assert(v.rows() == j.size() && "Test Data Size != Test Data Tag");
        V = v;
        J = j;
    }

    void write_colored_points();

private:
    std::string filepath;
    Eigen::MatrixXd V;
    std::array<double, TestSize> J;
};

#endif