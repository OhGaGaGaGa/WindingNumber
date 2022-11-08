#ifndef _winding_number_h
#define _winding_number_h

#include <tuple>
#include <vector>
#include <Eigen/Core>
#include "Meshs.h"
#include "Constants.h"

namespace generate_ramdom_points {
    inline double random_from_to(double left, double right) {
        return left + (right - left) * rand() / RAND_MAX;
    }
    Eigen::MatrixXd random_points(int cnt, double l0, double r0, double l1, double r1, double l2, double r2);
    Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V);
}

namespace generate_grid {
    inline int get_point_id(int i, int j, int k) {
        return (int)(i * (DIVI_Y + 1) * (DIVI_Z + 1) + j * (DIVI_Z + 1) + k);
    }
    std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> generate_grid(const Eigen::MatrixXd& inputV);
}

namespace winding_number {
    inline int calc_category(double x) {
        // assert(-1 - EPS < x && x < 1 + EPS && "winding value invalid. ");

        // Maybe can adjust condition order
        if (x < -1 + EPS) return 1;
        else if (-1 + EPS <= x && x < -EPS) return 2;
        else if (-EPS <= x && x <= EPS) return 3;
        else if (EPS < x && x <= 1 - EPS) return 4;
        else if (1 - EPS < x) return 5;
        return 0;
    }
    std::tuple<std::vector<double>, std::vector<int>> process_winding_number(const Eigen::MatrixXd& testV, const Meshs& meshs);
}

#endif