#include "IO.h"
#include "VTKwriter.h"
#include <iostream>
#include <algorithm>
#include <map>

struct Triple {
    int a, b, c;
    bool operator < (const Triple& rhs) const {
        if (a != rhs.a) return a < rhs.a;
        if (b != rhs.b) return b < rhs.b;
        return c < rhs.c;
    }
    bool operator == (const Triple& rhs) const {
        return a == rhs.a && b == rhs.b && c == rhs.c;
    }
};

Eigen::MatrixXi reorganize_faces(const Eigen::MatrixXi& inputF) {
    std::map<Triple, Triple> mp;
    for (auto i = 0; i < inputF.rows(); i++) {
        auto a = inputF(i, 0), b = inputF(i, 1), c = inputF(i, 2);
        auto min = std::min(a, std::min(b, c));
        auto mid = std::max(a,b)>c ? std::max(std::min(a,b),c) : std::max(a,b);
        auto max = std::max(a, std::max(b, c));
        Triple ordered = {min, mid, max};
        if (mp.count(ordered)) {
            mp[ordered] = {-1, -1, -1};
        }
        else {
            mp[ordered] = {a, b, c};
        }
    }
    std::vector<Triple> ret_vec;
    for (auto pair : mp) {
        if (!(pair.second == (Triple){-1, -1, -1}))
            ret_vec.push_back(pair.first);
    }
    Eigen::MatrixXi ret_matrix(ret_vec.size(), 3);
    for (auto i = 0; i < ret_vec.size(); i++) {
        ret_matrix.row(i) = (Eigen::Vector3i){ret_vec[i].a, ret_vec[i].b, ret_vec[i].c};
    }
    return ret_matrix;
}

namespace io {
    std::string input_filepath, output_input_mesh_filepath, test_point_filepath, winding_value_filepath, random_points_filepath, grid_filepath, iso_surface_filepath;

    bool file_path(int argc, char const* argv[]) {
        if (argc != 1 && argc != 1 + 7) {
            return false;
        }
// #ifndef INPUT_FILE_PATH_MANUALLY
        if (argc == 1) {
            std::string filename = "tiger";
            input_filepath = "../files/" + filename + "-in.obj";
            output_input_mesh_filepath = "../files/" + filename + "-input-mesh.obj";
            test_point_filepath = "../files/" + filename + "-test-out.txt";
            winding_value_filepath = "../files/" + filename + "-winding-number-out.txt";
            random_points_filepath = "../files/" + filename + "-points-out.vtk";
            grid_filepath = "../files/" + filename + "-grid-out.vtk";
            iso_surface_filepath = "../files/" + filename + "-isosurface-out.obj";
        }
// #else
        else {
            input_filepath = argv[1];
            output_input_mesh_filepath = argv[2];
            test_point_filepath = argv[3];
            winding_value_filepath = argv[4];
            random_points_filepath = argv[5];
            grid_filepath = argv[6];
            iso_surface_filepath = argv[7];
        }
// #endif
        return true;
    }

    bool input(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF, Eigen::MatrixXi& inputTet) {
        bool input_file_status = false;
#ifndef INPUT_VTK
        input_file_status = igl::readOBJ(input_filepath, inputV, inputF);
        std::cout << "inputV.rows(): " << inputV.rows() << ", inputF.rows(): " << inputF.rows() << "\n";
#else
        VTKreader reader(input_filepath.c_str());
        input_file_status = reader.file_status();
        if (input_file_status) {
            reader.read_from_file();
            inputV = reader.get_vertex();
            inputTet = reader.get_tet();

            auto tet_count = inputTet.rows();
            inputF.resize(tet_count << 2, 3);
            std::cout << tet_count << " " << inputF.rows() << " " << inputF.cols() << "\n";
            for (auto i = 0; i < tet_count; i++)
                for (auto j = 0; j < 4; j++)
                    inputF.row(i * 4 + j) = Eigen::Vector3i{inputTet(i, j % 4), inputTet(i, (j + 1) % 4), inputTet(i, (j + 2) % 4)};
            inputF = reorganize_faces(inputF);
        }
#endif
        return input_file_status;
    }

    bool output_test_points(const Eigen::MatrixXd& testV) {
        // TODO: argv, and open write error check
        std::ofstream out_test{ test_point_filepath };
        if (!out_test) return false;
        for (int i = 0; i < testV.rows(); i++) {
            out_test << "Point " << i << ": \t" << testV(i, 0) << " " << testV(i, 1) << " " << testV(i, 2) << '\n';
        }
        out_test << '\n';
        return true;
    }

    bool output_winding_numbers(const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge) {
        std::ofstream out_w{ winding_value_filepath };
        if (!out_w) return false;
        for (int i = 0; i < testV.rows(); i++) {
            out_w << w[i] << " " << judge[i] << '\n';
        }
        out_w << '\n';
        return true;
    }

    bool output_mesh_vtk(const Eigen::MatrixXd& testV, const std::vector<int>& judge) {
        VTKwriter writer(random_points_filepath.c_str(), testV, judge);
        bool output_file_status = writer.file_status();
        if (!output_file_status) return false;
        writer.write_colored_points();
        return true;
    }

    bool output_grid_vtk(const Eigen::MatrixXd& gridV, const Eigen::MatrixXi& gridCube, const std::vector<double>& wn) {
        VTKwriter writer(grid_filepath.c_str(), gridV, gridCube, wn);
        bool output_file_status = writer.file_status();
        if (!output_file_status) return false;
        writer.write_grid();
        return true;
    }
}