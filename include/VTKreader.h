#ifndef _vtkreader_h
#define _vtkreader_h

#include <fstream>
#include <string>
#include <cmath>
#include <utility>
#include <Eigen/Core>

class VTKreader {
public:
    VTKreader(const char* file) : _in(file) {}
    bool file_status() {
        return _in.is_open();
    }
    Eigen::MatrixXd get_vertex() { return _vertex; };
    Eigen::MatrixXi get_tet() { return _tet; };
    void read_from_file();

private:
    std::ifstream _in;
    Eigen::MatrixXd _vertex;
    Eigen::MatrixXi _tet;
};

#endif