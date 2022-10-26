#include "IO.h"
#include "VTKwriter.h"
#include "VTKreader.h"
#include <iostream>

namespace io {
    std::string input_filepath, test_point_filepath, winding_value_filepath, output_filepath;

    void file_path(int argc, char const* argv[]) {
    #ifndef INPUT_FILE_PATH_MANUALLY
        input_filepath = "../files/sample-in.vtk";
        test_point_filepath = "../files/sample-test-out.txt";
        winding_value_filepath = "../files/sample-judge-out.txt";
        output_filepath = "../files/sample-out.vtk";
    #else
        input_filepath = argv[1];
        test_point_filepath = argv[2];
        winding_value_filepath = argv[3];
        output_filepath = argv[4];
    #endif
    }

    bool input(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF) {
        bool input_file_status = false;
    #ifndef INPUT_VTK
        input_file_status = igl::readOBJ(input_filepath, inputV, inputF);
        std::cout << inputV.rows() << " " << inputF.rows() << " " << inputF.cols() << "\n";
    #else
        Eigen::MatrixXi inputTet;
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

    bool output_winding_numbers(const Eigen::MatrixXd& testV, const std::array<double, TESTSIZE>& w, const std::array<int, TESTSIZE>& judge) {
        std::ofstream out_w{ winding_value_filepath };
        if (!out_w) return false;
        for (int i = 0; i < testV.rows(); i++) {
            out_w << w[i] << " " << judge[i] << '\n';
        }
        out_w << '\n';
        return true;
    }

    bool output_vtk(const Eigen::MatrixXd& testV, const std::array<int, TESTSIZE>& judge) {
        VTKwriter writer(output_filepath.c_str(), testV, judge);
        bool output_file_status = writer.file_status();
        if (!output_file_status) return false;
        writer.write_colored_points();
        return true;
    }
}