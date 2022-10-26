#ifndef _io_h
#define _io_h

#include <string>
#include <Eigen/Dense>
#include <igl/readOBJ.h>

// #define INPUT_FILE_PATH_MANUALLY
#define INPUT_VTK

namespace io {
    extern std::string input_filepath, test_point_filepath, winding_value_filepath, random_points_filepath, iso_surface_filepath;

    bool file_path(int argc, char const* argv[]);
    bool input(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF, Eigen::MatrixXi& inputTet);
    bool output_test_points(const Eigen::MatrixXd& testV);
    bool output_winding_numbers(const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge);
    bool output_vtk(const Eigen::MatrixXd& testV, const std::vector<int>& judge);
}

#endif