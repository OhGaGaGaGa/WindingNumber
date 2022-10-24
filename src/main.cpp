#include <iostream>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>
#include <igl/solid_angle.h>
#include "Constants.h"
#include "Meshs.h"
#include "VTKwriter.h"

using namespace std;

Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V);

inline int calc_category(double x) {
    assert(-1 - EPS < x&& x < 1 + EPS && "winding value invalid. ");

    // Maybe can adjust condition order
    if (x < -1 + EPS) return 1;
    else if (-1 + EPS <= x && x < -EPS) return 2;
    else if (-EPS <= x && x <= EPS) return 3;
    else if (EPS < x && x <= 1 - EPS) return 4;
    else if (1 - EPS < x) return 5;
    return 0;
}

// #define INPUT_FILE_PATH_MANUALLY

int main(int argc, char const* argv[]) {
    std::string input_filepath, test_point_filepath, winding_value_filepath, output_filepath;

#ifndef INPUT_FILE_PATH_MANUALLY
    input_filepath = "../files/tiger-in.obj";
    test_point_filepath = "../files/tiger-test-out.txt";
    winding_value_filepath = "../files/tiger-judge-out.txt";
    output_filepath = "../files/tiger-out.vtk";
#endif
#ifdef INPUT_FILE_PATH_MANUALLY
    input_filepath = argv[1];
    test_point_filepath = argv[2];
    winding_value_filepath = argv[3];
    output_filepath = argv[4];
#endif

    Eigen::MatrixXd inputV;
    Eigen::MatrixXi inputF;

    // TODO: use argv to readin the input file name
    // BUG: no file reading/writing error check
    bool read_input_status = igl::readOBJ(input_filepath, inputV, inputF);
    if (!read_input_status) {
        cout << "Input File Error. \n";
        return 0;
    }

    Meshs meshs(inputV, inputF);

    auto testV = get_random_points(inputV);

    std::array<double, TESTSIZE> w{ 0 };
    std::array<int, TESTSIZE> judge{ 0 };

    for (int i = 0; i < testV.rows(); i++) {
        // TODO: w[i] = meshs.calc_winding_value(testV.row(i));
        Eigen::Vector3d tmpVec{ {testV(i, 0), testV(i, 1), testV(i, 2)} };
        // Eigen::Vector3d tmpVec{ testV.row(i) };
        w[i] = meshs.calc_winding_value(tmpVec);
        judge[i] = calc_category(w[i]);
    }
    cout << "hello\n";
    {
        // TODO: argv, and open write error check
        std::ofstream out_test{ test_point_filepath };
        for (int i = 0; i < testV.rows(); i++) {
            out_test << "Point " << i << ": \t" << testV(i, 0) << " " << testV(i, 1) << " " << testV(i, 2) << '\n';
        }
        out_test << '\n';
    }
    {
        std::ofstream out_w{ winding_value_filepath };
        for (int i = 0; i < testV.rows(); i++) {
            out_w << w[i] << " " << judge[i] << '\n';
        }
        out_w << '\n';
    }

    VTKwriter writer(output_filepath.c_str(), testV, judge);
    writer.write_colored_points();
    return 0;
}