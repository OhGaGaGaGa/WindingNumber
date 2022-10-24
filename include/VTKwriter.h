#ifndef _vtkwriter_h
#define _vtkwriter_h

#include <fstream>
#include <string>
#include <cmath>
#include <Eigen/Dense>
#include <utility>
#include "Constants.h"
#include "Eigen/src/Core/Matrix.h"

class VTKwriter {
public:
    VTKwriter(const char* file, const Eigen::MatrixXd& v, const std::array<int, TESTSIZE>& j) : _out(file), _vertex(v), _category(j) {
        assert(v.cols() == 3 && "Only support 3D data. ");
        assert(v.rows() == j.size() && "Test Data Size != Test Data Tag");
        assert(_out.is_open() && "Output path incorrect");
    }
    void write_colored_points();

private:
    std::ofstream _out;
    Eigen::MatrixXd _vertex;
    std::array<int, TESTSIZE> _category;
};

#endif