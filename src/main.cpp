#include <iostream>

#include <igl/marching_tets.h>
#include <igl/writeOBJ.h>
#include "Constants.h"
#include "Meshs.h"
#include "IO.h"

using namespace std;

Eigen::MatrixXd get_random_points(const Eigen::MatrixXd& V);

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

int main(int argc, char const* argv[]) {
    bool file_path_status = io::file_path(argc, argv);
    if (!file_path_status) {
        cout << "Invalid Arguments Count. " << '\n';
        return 0;
    }

    Eigen::MatrixXd inputV;
    Eigen::MatrixXi inputF;
    Eigen::MatrixXi inputTet;

    // TODO: use argv to readin the input file name
    // BUG: no file reading/writing error check
    bool input_file_status = io::input(inputV, inputF, inputTet);
    if (!input_file_status) {
        cout << "Input File Error. \n";
        return 0;
    }

    // auto testV = get_random_points(inputV);
    auto testV = inputV;

    bool output_file_status = io::output_test_points(testV);
    if (!output_file_status) {
        cout << "output_test_points Error. \n"; return 0;
    }

    Meshs meshs(inputV, inputF);
    std::vector<double> w (testV.rows());
    std::vector<int> judge(testV.rows());

    cout << "testV.rows(): " << testV.rows() << '\n';

    // Winding Number 
    for (int i = 0; i < testV.rows(); i++) {
        // TODO: w[i] = meshs.calc_winding_value(testV.row(i));
        Eigen::Vector3d tmpVec{ {testV(i, 0), testV(i, 1), testV(i, 2)} };
        // Eigen::Vector3d tmpVec{ testV.row(i) };
        w[i] = meshs.calc_winding_value(tmpVec);
        // if (w[i] < -1 - EPS || w[i] > 1 + EPS)
        //     cout << i << " " << w[i] << "\n";
        judge[i] = calc_category(w[i]);
    }
    cout << "Calculated Winding Number. \n";

    output_file_status = io::output_winding_numbers(testV, w, judge);
    if (!output_file_status) {
        cout << "output_winding_numbers Error. \n"; return 0;
    }

    output_file_status = io::output_vtk(testV, judge);
    if (!output_file_status) {
        cout << "output_vtk Error. \n"; return 0;
    }

    Eigen::MatrixXd isoV;
    Eigen::MatrixXi isoF;
    Eigen::VectorXd isoValue(w.size());
    for (auto i = 0; i < w.size(); i++) {
        isoValue(i) = w[i];
    }

    igl::marching_tets(inputV, inputTet, isoValue, ISO_VALUE, isoV, isoF);
    
    output_file_status = igl::writeOBJ(io::iso_surface_filepath, isoV, isoF);
    if (!output_file_status) {
        cout << "output_vtk Error. \n"; return 0;
    }
    
    return 0;
}