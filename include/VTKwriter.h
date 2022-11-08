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
    VTKwriter(const std::string& file, const Eigen::MatrixXd& v, const std::vector<int>& category) : 
        _out(file), _vertex(v), _category(category) {
            check_asserts();
    }


    VTKwriter(const std::string& file, const Eigen::MatrixXd& v, const Eigen::MatrixXi& grid, const std::vector<double>& wn) : 
        _out(file), _vertex(v), _grid(grid), _wn(wn) {
            check_asserts();
    }
    bool file_status() {
        return _out.is_open();
    }
    void write_colored_points();
    void write_grid();

private:
    std::ofstream _out;
    const Eigen::MatrixXd& _vertex;
    const Eigen::MatrixXi _grid;
    const std::vector<int> _category;
    const std::vector<double> _wn;
    void check_asserts() {
        assert(_vertex.cols() == 3 && "Only support 3D data. ");
        // assert(_vertex.rows() == _category.size() && "Test Data Size != Test Data Tag");
    }
};

#endif