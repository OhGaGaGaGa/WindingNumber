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
        // TODO: highly redundant code
        for (int i = 0; i < V.rows(); i++) {
            min0 = std::min(min0, V(i, 0));
            max0 = std::max(max0, V(i, 0));
            min1 = std::min(min1, V(i, 1));
            max1 = std::max(max1, V(i, 1));
            min2 = std::min(min2, V(i, 2));
            max2 = std::max(max2, V(i, 2));
        }
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
        return (int)(i * (DiviY + 1) * (DiviZ + 1) + j * (DiviZ + 1) + k);
    }

    void generate_grid(const Eigen::MatrixXd& inputV, Eigen::MatrixXd& gridV, Eigen::MatrixXi& gridCube) {
        find_min_max(inputV);
        // +- padding

        gridV.resize((DiviX + 1) * (DiviY + 1) * (DiviZ + 1), 3);
        for (int i = 0; i <= DiviX; i++) {
            double x = min0 + i * (max0 - min0) / DiviX;
            for (int j = 0; j <= DiviY; j++) {
                double y = min1 + j * (max1 - min1) / DiviY;
                for (int k = 0; k <= DiviZ; k++) {
                    double z = min2 + k * (max2 - min2) / DiviZ;
                    gridV.row(i * (DiviY + 1) * (DiviZ + 1) + j * (DiviZ + 1) + k) = (Eigen::Vector3d){x, y, z};
                }
            }
        }
        gridCube.resize(DiviX * DiviY * DiviZ, 8);
        for (int i = 0; i < DiviX; i++) {
            for (int j = 0; j < DiviY; j++) {
                for (int k = 0; k < DiviZ; k++) {
                    gridCube.row(i * DiviY * DiviZ + j * DiviZ + k) = (Eigen::Matrix<int, 8, 1>) {
                        // ID = 12: VTK_HEXAHEDRON
                        get_point_id(i, j, k), get_point_id(i, j, k + 1), get_point_id(i, j + 1, k + 1), get_point_id(i, j + 1, k),
                        get_point_id(i + 1, j, k), get_point_id(i + 1, j, k + 1), get_point_id(i + 1, j + 1, k + 1), get_point_id(i + 1, j + 1, k)
                    };
                }
            }
        }
    }

}