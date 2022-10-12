//
// Created by ybc on 22-7-5.
//
#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <variant>

#include "vtkGenericDataObjectReader.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkPointData.h"
#include "vtkNew.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"
#include "Eigen/Dense"
#include "spdlog/spdlog.h"

const std::vector<std::pair<int, int>> &construct_edges(int cell_type) {
    static const std::vector<std::pair<int, int>> hex_edges = {
            {0, 1},
            {1, 2},
            {2, 3},
            {3, 0},
            {4, 5},
            {5, 6},
            {6, 7},
            {7, 4},
            {0, 4},
            {1, 5},
            {2, 6},
            {3, 7}
    };
    static const std::vector<std::pair<int, int>> wedge_edges = {
//            {0,1},{1,2},{2,0},
//            {3,4},{4,5},{5,3},
            {0, 3},
            {1, 4},
            {2, 5},
    };
    static const std::vector<std::pair<int, int>> null_edges = {};
    if (cell_type == VTK_WEDGE) {
        return wedge_edges;
    } else if (cell_type == VTK_HEXAHEDRON) {
        return hex_edges;
    } else {
        return null_edges;
    }
}


int main(int argc, char *argv[]) {
    spdlog::set_pattern("%s:%#: %^[%l]%$ %v");
    // check arguments
    if (argc != 3) {
        spdlog::error("Usage: {} <input> <output>", argv[0]);
        return 1;
    }

    // Read hex-dominant mesh
    SPDLOG_INFO("Read hex-dominant mesh... (file name: {})", argv[1]);
    vtkNew<vtkGenericDataObjectReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    if (!reader->IsFileUnstructuredGrid()) {
        throw std::runtime_error("File is not a vtkUnstructuredGrid");
    }
    auto *ugrid = reader->GetUnstructuredGridOutput();
    auto point_data = ugrid->GetPointData()->GetScalars();
    auto P = ugrid->GetPoints();
    // find all hex cells or wedge cells
    // if abs(edge.v0.value - edge.v1.value) == 1, output the edge (l v0 v1)
    vtkSmartPointer<vtkCellArray> lineIndices = vtkSmartPointer<vtkCellArray>::New();
    for (vtkIdType i = 0; i < ugrid->GetNumberOfCells(); ++i) {
        // get the cell type
        auto cell = ugrid->GetCell(i);
        auto cell_type = cell->GetCellType();
        auto ids = cell->GetPointIds();
        for (const auto &edge: construct_edges(cell_type)) {
            auto v0 = ids->GetId(edge.first);
            auto v1 = ids->GetId(edge.second);
            auto point_data0 = int(point_data->GetTuple(v0)[0]);
            auto point_data1 = int(point_data->GetTuple(v1)[0]);
            if (std::abs(point_data0 - point_data1) == 1) {
                lineIndices->InsertNextCell(2);
                lineIndices->InsertCellPoint(v0);
                lineIndices->InsertCellPoint(v1);
            }else{
                assert(std::abs(point_data0 - point_data1) == 0);
            }
        }
    }

    SPDLOG_DEBUG("size of lineIndices: {}", lineIndices->GetNumberOfCells());

    vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
    data->SetPoints(P);
    data->SetLines(lineIndices);

    // write the output mesh
    SPDLOG_INFO("Write the output mesh... (file name: {})", argv[2]);
    vtkNew<vtkPolyDataWriter> writer;
    writer->SetFileVersion(42);
    writer->SetInputData(data);
    writer->SetFileName(argv[2]);
//    writer->WriteToOutputStringOn();
    writer->Write();
//    std::string ret = writer->GetOutputString();
}