#ifndef _vtkwriter_h
#define _vtkwriter_h

#include <fstream>
#include <string>
#include <cmath>
#include <Eigen/Dense>
#include "Constants.h"
#include "Eigen/src/Core/Matrix.h"

class VTKwriter {
public:
    VTKwriter(const char* file, Eigen::MatrixXd& v, const std::array<double, TestSize>& j) : filepath(file), J{ 0 } {
        assert(v.cols() == 3 && "Only support 3D data. ");
        assert(v.rows() == j.size() && "Test Data Size != Test Data Tag");
        V = v;
        J = j;

        // min_sa = INT_MAX, max_sa = INT_MIN;
        // for (auto a : J) {
        //     min_sa = std::min(min_sa, a);
        //     max_sa = std::max(max_sa, a);
        // }

        // std::nth_element(j.begin(), j.begin() + 10, j.end());
        // min_sa = j[10];
        // std::nth_element(j.begin(), j.begin() + 10, j.end(), std::greater<>());
        // max_sa = j[10];
    }

    void write_colored_points();

private:
    std::string filepath;
    Eigen::MatrixXd V;
    std::array<double, TestSize> J;

    double min_sa, max_sa;

    int calc_category(double x) {
        if (x < min_sa) return 0;
        if (x > max_sa) return (int)Groups;
        return (int)(Groups * (x - min_sa) / (max_sa - min_sa));
    }
};

#endif