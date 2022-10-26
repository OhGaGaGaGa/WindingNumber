#ifndef _vtkwriter_h
#define _vtkwriter_h

#include <fstream>
#include <string>
#include <cmath>
#include <Eigen/Dense>
#include <utility>
#include "Eigen/src/Core/Matrix.h"

class VTKwriter {
public:
    VTKwriter(const char* file, const Eigen::MatrixXd& v, const std::vector<int>& j) : _out(file), _vertex(v), _category(j) {
        assert(v.cols() == 3 && "Only support 3D data. ");
        assert(v.rows() == j.size() && "Test Data Size != Test Data Tag");
    }
    bool file_status() {
        return _out.is_open();
    }
    void write_colored_points();

private:
    std::ofstream _out;
    const Eigen::MatrixXd _vertex;
    const std::vector<int> _category;
};

#endif