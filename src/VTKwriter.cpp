#include "VTKwriter.h"

void VTKwriter::write_colored_points() {
    std::ofstream out{ filepath.c_str() };
    using std::endl;

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

    for (int i = 0; i < V.rows(); i++) {
        if (J[i] < -eps)
            out << 1 << endl;
        else if (-eps <= J[i] && J[i] <= eps)
            out << 2 << endl;
        else
            out << 3 << endl;
    }
}