#pragma once

#include <vtkCellArray.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkNew.h>

#include "celltypes.hpp"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkType.h"

#include <Eigen/Core>
#include <string>

namespace lspadding {
inline std::tuple<Eigen::VectorXi, Eigen::MatrixXd, std::vector<Tet>> read_vtk(
    const std::string &filename) {

    using namespace std::string_literals;
    vtkNew<vtkGenericDataObjectReader> reader;
    reader->SetFileName(filename.c_str());
    reader->Update();
    if (!reader->IsFileUnstructuredGrid()) {
        throw std::runtime_error("File is not a vtkUnstructuredGrid");
    }
    auto *ugrid = reader->GetUnstructuredGridOutput();

    auto nv = ugrid->GetNumberOfPoints();
    auto nc = ugrid->GetNumberOfCells();

    for (int i = 0; i < nc; ++i) {
        if (ugrid->GetCellType(i) != VTK_TETRA) {
            throw std::runtime_error("File contains non-tetrahedral cells");
        }
    }
    auto P = ugrid->GetPoints();
    auto C = ugrid->GetCells();

    Eigen::MatrixXd pos = Eigen::MatrixXd::Zero(nv, 3);

    for (int i = 0; i < nv; ++i) {
        Eigen::Vector3d V;
        P->GetPoint(i, V.data());
        pos.row(i) = V;
    }

    std::vector<Tet> tets;
    tets.reserve(nc);

    for (int i = 0; i < nc; ++i) {
        auto c = ugrid->GetCell(i);
        std::array<Vertex, 4> v{};
        for (int j = 0; j < 4; ++j) {
            v[j] = vertex_at(c->GetPointId(j), nv);
        }
        tets.push_back(Tet::make(v));
    }

    auto point_data = ugrid->GetPointData();
    int padding_id  = -1;
    for (int i = 0; i < point_data->GetNumberOfArrays(); i++) {
        if (point_data->GetArrayName(i) == "padding"s) {
            padding_id = i;
            break;
        }
    }
    if (padding_id == -1) {
        throw std::runtime_error("File does not contain padding info");
    }
    auto padding      = point_data->GetArray(padding_id);
    auto padding_data = vtkIntArray::SafeDownCast(padding);
    if (padding_data == nullptr) {
        throw std::runtime_error("Padding is not of type int");
    }
    Eigen::VectorXi padding_vec(nv);
    for (int i = 0; i < nv; ++i) {
        padding_vec(i) = padding_data->GetValue(i);
    }
    return {std::move(padding_vec), std::move(pos), std::move(tets)};
}
} // namespace lspadding