#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Eigen/Core>
#include "Constants.h"

namespace ramdom_point_and_generate_grid {

    // TODO: inline
    inline double random_from_to(double left, double right) {
        return left + (right - left) * rand() / RAND_MAX;
    }

    Eigen::MatrixXd random_points(int cnt, double l0, double r0, double l1, double r1, double l2, double r2) {
        Eigen::MatrixXd ret(cnt, 3);
        for (int i = 0; i < cnt; i++) {
            ret(i, 0) = random_from_to(l0, r0);
            ret(i, 1) = random_from_to(l1, r1);
            ret(i, 2) = random_from_to(l2, r2);
        }
        return ret;
    }

    double min0 = INT_MAX, max0 = INT_MIN;
    double min1 = INT_MAX, max1 = INT_MIN;
    double min2 = INT_MAX, max2 = INT_MIN;

    void find_min_max(const Eigen::MatrixXd& V) {
        min0 = V.col(0).minCoeff();
        max0 = V.col(0).maxCoeff();
        min1 = V.col(1).minCoeff();
        max1 = V.col(1).maxCoeff();
        min2 = V.col(2).minCoeff();
        max2 = V.col(2).maxCoeff();

        // TODO: highly redundant code
        // for (int i = 0; i < V.rows(); i++) {
        //     min0 = std::min(min0, V(i, 0));
        //     max0 = std::max(max0, V(i, 0));
        //     min1 = std::min(min1, V(i, 1));
        //     max1 = std::max(max1, V(i, 1));
        //     min2 = std::min(min2, V(i, 2));
        //     max2 = std::max(max2, V(i, 2));
        // }
    }

    Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V) {
        srand(time(nullptr));
        find_min_max(V);

        return random_points(TESTSIZE,
            min0 - PADDING, max0 + PADDING,
            min1 - PADDING, max1 + PADDING,
            min2 - PADDING, max2 + PADDING
        );
    }

    inline int get_point_id(int i, int j, int k) {
        return (int)(i * (DIVI_Y + 1) * (DIVI_Z + 1) + j * (DIVI_Z + 1) + k);
    }

    void generate_grid(const Eigen::MatrixXd& inputV, Eigen::MatrixXd& gridV, Eigen::MatrixXi& gridCube) {
        find_min_max(inputV);
        // +- padding

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
    }

}