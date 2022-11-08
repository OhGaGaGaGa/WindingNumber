#ifndef _io_h
#define _io_h

#include <string>
#include <igl/readOBJ.h>
#include <igl/writeOBJ.h>

class IO {
public:
    IO() = delete;
    IO(int argc, const std::vector<std::string>& filepath);
    bool input_triangle_mesh(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF);
    bool input_tet_mesh(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF, Eigen::MatrixXi& inputTet);
    bool output_files(const Eigen::MatrixXd& inputV, const Eigen::MatrixXi& inputF, const Eigen::MatrixXi& inputTet, 
        const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge, 
        const Eigen::MatrixXd& gridV, const Eigen::MatrixXi& gridCube);

private:
    std::string _input_filepath, _output_input_mesh_filepath, _test_point_filepath, _winding_value_filepath, _random_points_filepath, _grid_filepath, _iso_surface_filepath;
    std::tuple<bool, Eigen::MatrixXd, Eigen::MatrixXi> read_tet_from_vtk();
    bool output_test_points(const Eigen::MatrixXd& testV);
    bool output_winding_numbers(const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge);
    bool output_mesh_vtk(const Eigen::MatrixXd& testV, const std::vector<int>& judge);
    bool output_grid_vtk(const Eigen::MatrixXd& gridV, const Eigen::MatrixXi& gridCube, const std::vector<double>& wn);
};

#endif