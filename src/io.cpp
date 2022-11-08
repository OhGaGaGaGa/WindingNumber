#include "IO.h"
#include "VTKwriter.h"
#include <iostream>
#include <algorithm>
#include <map>

// std::tuple
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
        if (!(pair.second == Triple{-1, -1, -1}))
            ret_vec.push_back(pair.first);
    }
    Eigen::MatrixXi ret_matrix(ret_vec.size(), 3);
    for (auto i = 0; i < ret_vec.size(); i++) {
        ret_matrix.row(i) = (Eigen::Vector3i){ret_vec[i].a, ret_vec[i].b, ret_vec[i].c};
    }
    return ret_matrix;
}

IO::IO(int argc, const std::vector<std::string>& filepath) {
    if (argc == 1) {
        std::string filename = "tiger";
        _input_filepath = "../files/" + filename + "-in.obj";
        _output_input_mesh_filepath = "../files/" + filename + "-input-mesh.obj";
        _test_point_filepath = "../files/" + filename + "-test-out.txt";
        _winding_value_filepath = "../files/" + filename + "-winding-number-out.txt";
        _random_points_filepath = "../files/" + filename + "-points-out.vtk";
        _grid_filepath = "../files/" + filename + "-grid-out.vtk";
        _iso_surface_filepath = "../files/" + filename + "-isosurface-out.obj";
    }
    else {
        _input_filepath = filepath[0];
        _output_input_mesh_filepath = filepath[1];
        _test_point_filepath = filepath[2];
        _winding_value_filepath = filepath[3];
        _random_points_filepath = filepath[4];
        _grid_filepath = filepath[5];
        _iso_surface_filepath = filepath[6];
    }
}

bool IO::input_triangle_mesh(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF) {
    bool input_file_status = igl::readOBJ(_input_filepath, inputV, inputF);
    if (input_file_status) std::cout << "inputV.rows(): " << inputV.rows() << ", inputF.rows(): " << inputF.rows() << "\n";
    return input_file_status;
}

bool IO::input_tet_mesh(Eigen::MatrixXd& inputV, Eigen::MatrixXi& inputF, Eigen::MatrixXi& inputTet) {
    bool read_status = false;
    std::tie(read_status, inputV, inputTet) = read_tet_from_vtk();
    if (read_status) {
        auto tet_count = inputTet.rows();
        inputF.resize(tet_count << 2, 3);
        std::cout << tet_count << " " << inputF.rows() << " " << inputF.cols() << "\n";
        for (auto i = 0; i < tet_count; i++)
            for (auto j = 0; j < 4; j++)
                inputF.row(i * 4 + j) = Eigen::Vector3i{inputTet(i, j % 4), inputTet(i, (j + 1) % 4), inputTet(i, (j + 2) % 4)};
        inputF = reorganize_faces(inputF);
        std::cout << "inputV.rows(): " << inputV.rows() << ", inputF.rows(): " << inputF.rows() << "\n";
    }
    return read_status;
}

std::tuple<bool, Eigen::MatrixXd, Eigen::MatrixXi> IO::read_tet_from_vtk() {
    std::ifstream in(_input_filepath);
    bool input_file_status = in.is_open();
    Eigen::MatrixXd vertex;
    Eigen::MatrixXi tet;

    std::string line;
    std::getline(in, line); // # vtk DataFile Version 2.0
    std::getline(in, line); // FILE_NAME, Created by NAME
    std::getline(in, line); // ASCII
    std::getline(in, line); // DATASET UNSTRUCTURED_GRID
    int vertex_count = 0;
    in >> line >> vertex_count >> line; // POINTS COUNT double

    vertex.resize(vertex_count, 3);

    for (auto i = 0; i < vertex_count; i++) {
        double x = 0, y = 0, z = 0;
        in >> x >> y >> z;
        vertex.row(i) = Eigen::Vector3d{ x, y, z };
    }

    int tet_count = 0, tet_input_count = 0;
    in >> line >> tet_count >> tet_input_count;
    if (tet_count * 5 != tet_input_count) {
        std::cout << "Tet Count doesn't match\n";
        return std::make_tuple(false, Eigen::MatrixXd(), Eigen::MatrixXi());
    }
    tet.resize(tet_count, 4);
    for (auto i = 0; i < tet_count; i++) {
        int four = 4, v1 = 0, v2 = 0, v3 = 0, v4 = 0;
        in >> four >> v1 >> v2 >> v3 >> v4;
        tet.row(i) = Eigen::Vector4i{ v1, v2, v3, v4 };
    }
    return std::make_tuple(true, vertex, tet);
}

bool IO::output_test_points(const Eigen::MatrixXd& testV) {
    // TODO: argv, and open write error check
    std::ofstream out_test{ _test_point_filepath };
    if (!out_test) return false;
    for (int i = 0; i < testV.rows(); i++) {
        out_test << "Point " << i << ": \t" << testV(i, 0) << " " << testV(i, 1) << " " << testV(i, 2) << '\n';
    }
    out_test << '\n';
    return true;
}

bool IO::output_winding_numbers(const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge) {
    std::ofstream out_w{ _winding_value_filepath };
    if (!out_w) return false;
    for (int i = 0; i < testV.rows(); i++) {
        out_w << w[i] << " " << judge[i] << '\n';
    }
    out_w << '\n';
    return true;
}

bool IO::output_mesh_vtk(const Eigen::MatrixXd& testV, const std::vector<int>& judge) {
    VTKwriter writer(_random_points_filepath, testV, judge);
    bool output_file_status = writer.file_status();
    if (!output_file_status) return false;
    writer.write_colored_points();
    return true;
}

bool IO::output_grid_vtk(const Eigen::MatrixXd& gridV, const Eigen::MatrixXi& gridCube, const std::vector<double>& wn) {
    VTKwriter writer(_grid_filepath, gridV, gridCube, wn);
    bool output_file_status = writer.file_status();
    if (!output_file_status) return false;
    writer.write_grid();
    return true;
}

bool IO::output_files(const Eigen::MatrixXd& inputV, const Eigen::MatrixXi& inputF, const Eigen::MatrixXi& inputTet, 
    const Eigen::MatrixXd& testV, const std::vector<double>& w, const std::vector<int>& judge, 
    const Eigen::MatrixXd& gridV, const Eigen::MatrixXi& gridCube
) {
    using std::cout;

    bool output_file_status = igl::writeOBJ(_output_input_mesh_filepath, inputV, inputF);
    if (!output_file_status) {
        cout << "output_input_mesh Error. \n"; return false;
    }
    output_file_status = output_test_points(testV);
    if (!output_file_status) {
        cout << "output_test_points Error. \n"; return false;
    }
    output_file_status = output_winding_numbers(testV, w, judge);
    if (!output_file_status) {
        cout << "output_winding_numbers Error. \n"; return false;
    }
    output_file_status = output_mesh_vtk(testV, judge);
    if (!output_file_status) {
        cout << "output_mesh_vtk Error. \n"; return false;
    }
    output_file_status = output_grid_vtk(gridV, gridCube, w);
    if (!output_file_status) {
        cout << "output_grid_vtk Error. \n"; return false;
    }

    // Manually output iso-surface mesh
    // only support input VTK
    
    // Eigen::MatrixXd isoV;
    // Eigen::MatrixXi isoF;
    // Eigen::VectorXd isoValue(w.size());
    // for (auto i = 0; i < w.size(); i++) 
    //     isoValue(i) = w[i];
    // igl::marching_tets(inputV, inputTet, isoValue, ISO_VALUE, isoV, isoF);
    
    // output_file_status = igl::writeOBJ(_iso_surface_filepath, isoV, isoF);
    // if (!output_file_status) {
    //     cout << "output_iso_surface Error. \n"; return false;
    // }
    
    return true;
}