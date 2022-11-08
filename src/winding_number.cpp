// Rename this file

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "WindingNumber.h"

namespace generate_ramdom_points {
    Eigen::MatrixXd random_points(int cnt, double l0, double r0, double l1, double r1, double l2, double r2) {
        Eigen::MatrixXd ret(cnt, 3);
        for (int i = 0; i < cnt; i++) {
            ret(i, 0) = random_from_to(l0, r0);
            ret(i, 1) = random_from_to(l1, r1);
            ret(i, 2) = random_from_to(l2, r2);
        }
        return ret;
    }

    Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V) {
        srand(time(nullptr));
        double min0 = V.col(0).minCoeff();
        double max0 = V.col(0).maxCoeff();
        double min1 = V.col(1).minCoeff();
        double max1 = V.col(1).maxCoeff();
        double min2 = V.col(2).minCoeff();
        double max2 = V.col(2).maxCoeff();

        return random_points(TESTSIZE,
            min0 - PADDING, max0 + PADDING,
            min1 - PADDING, max1 + PADDING,
            min2 - PADDING, max2 + PADDING
        );
    }
}

namespace generate_grid {
    std::tuple<Eigen::MatrixXd, Eigen::MatrixXi> generate_grid(const Eigen::MatrixXd& inputV) {
        Eigen::MatrixXd gridV;
        Eigen::MatrixXi gridCube;
        double min0 = inputV.col(0).minCoeff() - PADDING;
        double max0 = inputV.col(0).maxCoeff() + PADDING;
        double min1 = inputV.col(1).minCoeff() - PADDING;
        double max1 = inputV.col(1).maxCoeff() + PADDING;
        double min2 = inputV.col(2).minCoeff() - PADDING;
        double max2 = inputV.col(2).maxCoeff() + PADDING;

        gridV.resize((DIVI_X + 1) * (DIVI_Y + 1) * (DIVI_Z + 1), 3);
        for (int i = 0; i <= DIVI_X; i++) {
            double x = min0 + i * (max0 - min0) / DIVI_X;
            for (int j = 0; j <= DIVI_Y; j++) {
                double y = min1 + j * (max1 - min1) / DIVI_Y;
                for (int k = 0; k <= DIVI_Z; k++) {
                    double z = min2 + k * (max2 - min2) / DIVI_Z;
                    gridV.row(i * (DIVI_Y + 1) * (DIVI_Z + 1) + j * (DIVI_Z + 1) + k) = (Eigen::Vector3d){x, y, z};
                }
            }
        }
        gridCube.resize(DIVI_X * DIVI_Y * DIVI_Z, 8);
        for (int i = 0; i < DIVI_X; i++) {
            for (int j = 0; j < DIVI_Y; j++) {
                for (int k = 0; k < DIVI_Z; k++) {
                    gridCube.row(i * DIVI_Y * DIVI_Z + j * DIVI_Z + k) = (Eigen::Matrix<int, 8, 1>) {
                        // ID = 12: VTK_HEXAHEDRON
                        get_point_id(i, j, k), get_point_id(i, j, k + 1), get_point_id(i, j + 1, k + 1), get_point_id(i, j + 1, k),
                        get_point_id(i + 1, j, k), get_point_id(i + 1, j, k + 1), get_point_id(i + 1, j + 1, k + 1), get_point_id(i + 1, j + 1, k)
                    };
                }
            }
        }
        return std::make_tuple(gridV, gridCube);
    }
}

namespace winding_number {
    std::tuple<std::vector<double>, std::vector<int>> process_winding_number(const Eigen::MatrixXd& testV, const Meshs& meshs){
        std::vector<double> w (testV.rows());
        std::vector<int> judge(testV.rows());

        std::cout << "testV.rows(): " << testV.rows() << '\n';

        // Winding Number 
        for (int i = 0; i < testV.rows(); i++) {
            w[i] = meshs.calc_winding_value_using_octree(testV.row(i));
            judge[i] = calc_category(w[i]);
        }
        std::cout << "Calculated Winding Number. \n";

        return std::make_tuple(w, judge);
    }
}