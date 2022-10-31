#include "VTKwriter.h"
#include <iostream>

void VTKwriter::write_colored_points() {
    using std::endl;

    _out << "# vtk DataFile Version 2.0" << endl;
    _out << "vtk output" << endl;
    _out << "ASCII" << endl;
    _out << "DATASET UNSTRUCTURED_GRID" << endl;

    _out << "POINTS " << _vertex.rows() << " double" << endl;
    for (int i = 0; i < _vertex.rows(); i++) {
        for (int j = 0; j < _vertex.cols(); j++)
            _out << _vertex(i, j) << " ";
        _out << endl;
    }
    _out << endl;

    _out << "CELLS " << _vertex.rows() << " " << _vertex.rows() * 2 << endl;
    for (int i = 0; i < _vertex.rows(); i++) {
        _out << "1 " << i << endl;
    }
    _out << endl;

    _out << "CELL_TYPES " << _vertex.rows() << endl;
    for (int i = 0; i < _vertex.rows(); i++)
        _out << 1 << endl;
    _out << endl;

    _out << "POINT_DATA " << _vertex.rows() << endl;
    _out << "SCALARS color double 1" << endl;
    _out << "LOOKUP_TABLE default" << endl;

    for (auto i = 0; i < _vertex.rows(); i++) {
        _out << _category[i] << endl;
    }
}

void VTKwriter::write_grid() {
    using std::endl;

    _out << "# vtk DataFile Version 2.0" << endl;
    _out << "vtk output" << endl;
    _out << "ASCII" << endl;
    _out << "DATASET UNSTRUCTURED_GRID" << endl;

    _out << "POINTS " << _vertex.rows() << " double" << endl;
    for (int i = 0; i < _vertex.rows(); i++) {
        for (int j = 0; j < _vertex.cols(); j++)
            _out << _vertex(i, j) << " ";
        _out << endl;
    }
    _out << endl;

    _out << "CELLS " << _grid.rows() << " " << _grid.rows() * 9 << endl;
    for (int i = 0; i < _grid.rows(); i++) {
        _out << "8";
        for (auto val : _grid.row(i)) {
            _out << " " << val;
        }
        _out << endl;
    }
    _out << endl;

    _out << "CELL_TYPES " << _grid.rows() << endl;
    for (int i = 0; i < _grid.rows(); i++)
        _out << 12 << endl;
    _out << endl;

    _out << "POINT_DATA " << _vertex.rows() << endl;
    _out << "SCALARS WindingNumber double 1" << endl;
    _out << "LOOKUP_TABLE default" << endl;

    for (auto i = 0; i < _vertex.rows(); i++) {
        _out << _wn[i] << endl;
    }
}