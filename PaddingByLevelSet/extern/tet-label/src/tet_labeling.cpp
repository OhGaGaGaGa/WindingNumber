#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <string>
#include <queue>

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "spdlog/spdlog.h"
#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMeshTopologyKernel.hh>

#include "vtkGenericDataObjectReader.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGridWriter.h"

#include "labels.hpp"
#include "config.hpp"
#include "vtk_io.h"
#include "tetrahedralize.hpp"
#include "remove_outside_cells.hpp"
#include "format_converter.hpp"

using SurfaceMesh = OpenMesh::TriMesh_ArrayKernelT<>;
using TetMesh     = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Geometry::Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>;

void ReadVTK(const std::string &file,
             Eigen::Matrix3Xd &V,
             Eigen::Matrix3Xi &F,
             Eigen::VectorXi &face_labels) {
    vtkNew<vtkGenericDataObjectReader> reader;
    reader->SetFileName(file.c_str());
    reader->Update();
    if (!reader->IsFileUnstructuredGrid()) {
        throw std::runtime_error("File is not a vtkUnstructuredGrid");
    }
    auto *ugrid            = reader->GetUnstructuredGridOutput();
    auto surface_cell_data = ugrid->GetCellData()->GetArray(0);
    if (surface_cell_data == nullptr) {
        throw std::runtime_error("surface mesh has no cell data (or the cell data is not an integer array)");
    }

    int num_points = ugrid->GetNumberOfPoints();
    int num_cells  = ugrid->GetNumberOfCells();

    V.resize(3, num_points);
    for (int i = 0; i < num_points; ++i) {
        Eigen::Vector3d p;
        ugrid->GetPoint(i, p.data());
        V.col(i) = p;
    }

    F.resize(3, num_cells);
    for (vtkIdType i = 0; i < num_cells; ++i) {
        auto cell = ugrid->GetCell(i);
        auto ids  = cell->GetPointIds();
        F.col(i)  = Eigen::Vector3i{ids->GetId(0), ids->GetId(1), ids->GetId(2)};
    }

    face_labels.resize(num_cells);
    auto cell_data = ugrid->GetCellData()->GetArray(0);
    for (vtkIdType i = 0; i < num_cells; ++i) {
        face_labels(i) = int(cell_data->GetTuple1(i));
    }
}

double AverageEdgeLength(const SurfaceMesh &mesh) {
    double avg_length = 0;
    for (auto edge : mesh.edges()) {
        avg_length += mesh.calc_edge_length(edge);
    }
    return avg_length / mesh.n_edges();
}

int main(int argc, char *argv[]) {
    spdlog::set_pattern("%s:%#: %^[%l]%$ %v");
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    // parameters
    CLI::App app("Tetrahedralize a surface mesh with given labels");
    std::string input_surface_vtk_path;
    std::string input_config_path;
    std::string output_volume_vtk_path;
    double alpha = 5.0;
    app.add_option("-i,--input", input_surface_vtk_path,
                   "The vtk file path of the input surface mesh.")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-c,--config", input_config_path,
                   "The config file path of the input.")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("-o,--output", output_volume_vtk_path,
                   "The vtk file path of the output volume mesh.")
        ->required();
    app.add_option("-a,--alpha", alpha,
                   "The max volume of tetrahedron in the output mesh = alpha * average_edge_length");
    CLI11_PARSE(app, argc, argv);
    SPDLOG_INFO("The path of the input vkt: {}", input_surface_vtk_path);
    SPDLOG_INFO("The path of the input config: {}", input_config_path);
    SPDLOG_INFO("The path of the output vtk: {}", output_volume_vtk_path);

    // read the input surface mesh
    SPDLOG_INFO("Read vtk file...");
    Eigen::Matrix3Xd V;
    Eigen::Matrix3Xi F;
    Eigen::VectorXi face_labels;
    ReadVTK(input_surface_vtk_path, V, F, face_labels);
    SPDLOG_INFO("number of points in input: {}", V.cols());
    SPDLOG_INFO("number of faces in input: {}", F.cols());

    // read the config file and extract the information
    Config config                   = ReadConfig(input_config_path);
    std::vector<int> layer_at_cells = tetlabel::ExtractBoundaryLayersInfo(F, face_labels, config);

    // convert the input surface mesh to OpenMesh instance
    SurfaceMesh surface_mesh = tetlabel::Convert(V, F);

    // interpolate the labels (cell -> vertex)
    std::vector<int> layer_at_points = tetlabel::Interpolate(surface_mesh, layer_at_cells);

    // mark connected components
    auto component_ids = tetlabel::MarkConnectedComponents(surface_mesh);

    // tetrahedralize the surface mesh
    SPDLOG_INFO("avg edge length: {}", AverageEdgeLength(surface_mesh));

    auto tet_mesh = mesh_tools::Tetrahedralize(V, F, alpha * AverageEdgeLength(surface_mesh));

    // remove cells out of the boundary
    // ! the following procedure works because mesh_tools::Tetrahedralize()
    // ! does not change the order of the vertices from the input surface mesh.
    tetlabel::RemoveOutsideCells(tet_mesh, component_ids);

    // output the tet mesh
    auto tet_mesh_vtk = tetlabel::Convert<OpenVolumeMesh::Geometry::Vec3d>(tet_mesh);
    SPDLOG_INFO("Write vtk file...");
    mesh_tools::io::WriteVtk(tet_mesh_vtk, output_volume_vtk_path, layer_at_points);

    SPDLOG_INFO("Done.");
    return 0;
}