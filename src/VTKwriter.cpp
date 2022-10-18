#include "VTKwriter.h"
#include <iostream>

void VTKwriter::write_colored_points() {
    std::ofstream out{ filepath.c_str() };
    using std::endl;

    assert(out.is_open() && "Output path incorrect");

    out << "# vtk DataFile Version 2.0" << endl;
    out << "vtk output" << endl;
    out << "ASCII" << endl;
    out << "DATASET UNSTRUCTURED_GRID" << endl;

    out << "POINTS " << V.rows() << " double" << endl;
    for (int i = 0; i < V.rows(); i++) {
        for (int j = 0; j < V.cols(); j++)
            out << V(i, j) << " ";
        out << endl;
    }
    out << endl;

    out << "CELLS " << V.rows() << " " << V.rows() * 2 << endl;
    for (int i = 0; i < V.rows(); i++) {
        out << "1 " << i << endl;
    }
    out << endl;

    out << "CELL_TYPES " << V.rows() << endl;
    for (int i = 0; i < V.rows(); i++)
        out << 1 << endl;
    out << endl;

    out << "POINT_DATA " << V.rows() << endl;
    out << "SCALARS color double 1" << endl;
    out << "LOOKUP_TABLE default" << endl;

    std::vector<int> sort_arr(V.rows());

    std::cout << "Min sa = " << min_sa << ", Max sa = " << max_sa << std::endl;

    int min_tag = INT_MAX;
    for (int i = 0; i < V.rows(); i++) {
        sort_arr[i] = J[i]; // calc_category(J[i]);
        min_tag = std::min(min_tag, sort_arr[i]);
        // std::cout << J[i] << " " << calc_category(J[i]) << endl;
    }

    std::cout << min_tag << endl;

    for (int i = 0; i < V.rows(); i++) {
        out << sort_arr[i] - min_tag << endl;
        // std::cout << (sort_arr[i] - min_tag) << " ";
    }
    // std::cout << endl;

    std::ofstream cate_out{ "../files/cate_out.txt" };
    for (auto a : sort_arr) {
        cate_out << a << " ";
    }
    cate_out << std::endl;
}