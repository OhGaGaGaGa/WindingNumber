#include "VTKreader.h"

#include <string>
#include <fstream>
#include <iostream>
#include <exception>

void VTKreader::read_from_file() {
    std::string line;
    std::getline(_in, line); // # vtk DataFile Version 2.0
    std::getline(_in, line); // FILE_NAME, Created by NAME
    std::getline(_in, line); // ASCII
    std::getline(_in, line); // DATASET UNSTRUCTURED_GRID
    int vertex_count = 0;
    _in >> line >> vertex_count >> line; // POINTS COUNT double

    _vertex.resize(vertex_count, 3);

    for (auto i = 0; i < vertex_count; i++) {
        double x = 0, y = 0, z = 0;
        _in >> x >> y >> z;
        _vertex.row(i) = Eigen::Vector3d{ x, y, z };
    }

    int tet_count = 0, tet_input_count = 0;
    _in >> line >> tet_count >> tet_input_count;
    if (tet_count * 5 != tet_input_count) {
        std::cout << "Tet Count doesn't match\n";
        return;
    }
    _tet.resize(tet_count, 4);
    for (auto i = 0; i < tet_count; i++) {
        int four = 4, v1 = 0, v2 = 0, v3 = 0, v4 = 0;
        _in >> four >> v1 >> v2 >> v3 >> v4;
        _tet.row(i) = Eigen::Vector4i{ v1, v2, v3, v4 };
    }
}