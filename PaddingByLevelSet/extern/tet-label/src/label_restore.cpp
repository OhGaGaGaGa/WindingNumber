//
// Created by ybc on 22-7-8.
//

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "vtkGenericDataObjectReader.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellType.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkUnstructuredGridWriter.h"
#include "spdlog/spdlog.h"


const std::vector<std::vector<int>> &construct_faces(int cell_type) {
    static const std::vector<std::vector<int>> hex_faces = {
            {0, 1, 2, 3},
            {4, 5, 6, 7},
            {0, 1, 5, 4},
            {3, 2, 6, 7},
            {0, 3, 7, 4},
            {1, 2, 6, 5}
    };
    static const std::vector<std::vector<int>> wedge_faces = {
            {0, 1, 2},
            {3, 4, 5},
            {0, 2, 5, 3},
            {1, 0, 3, 4},
            {2, 1, 4, 5},
    };
    static const std::vector<std::vector<int>> tet_faces = {
            {0, 1, 2},
            {0, 3, 1},
            {0, 2, 3},
            {2, 3, 1},
    };
    static const std::vector<std::vector<int>> null_faces = {};

    if (cell_type == VTK_WEDGE) {
        return wedge_faces;
    } else if (cell_type == VTK_HEXAHEDRON) {
        return hex_faces;
    } else if (cell_type == VTK_TETRA) {
        return tet_faces;
    } else {
        throw std::runtime_error("not implemented face type!");
    }
}

template<typename Out>
void split(const std::string &s, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, ' ')) {
        *result++ = std::stoi(item);
    }
}

std::vector<int> split(const std::string &s) {
    std::vector<int> elems;
    split(s, std::back_inserter(elems));
    return elems;
}

std::vector<std::vector<int>> index_vector(std::ifstream &index_fs) {
    std::vector<std::vector<int>> B;
    for (std::string line; getline(index_fs, line);) {
        B.emplace_back(split(line));
    }
    return B;
}

std::vector<std::vector<vtkIdType>> get_faces(vtkCell *cell) {
    std::vector<std::vector<vtkIdType>> faces;
    auto cell_type = cell->GetCellType();
    const auto &faces_indices = construct_faces(cell_type);
    for (const auto &face_indices: faces_indices) {
        std::vector<vtkIdType> face;
        for (int face_index: face_indices) {
            face.emplace_back(cell->GetPointId(face_index));
        }
        if (cell_type == VTK_WEDGE) {
            assert(face.size() == 4 || face.size() == 3);
        } else if (cell_type == VTK_HEXAHEDRON) {
            assert(face.size() == 4);
        } else if (cell_type == VTK_TETRA) {
            assert(face.size() == 3);
        }
        faces.emplace_back(face);
    }
    if (cell_type == VTK_WEDGE) {
        assert(faces.size() == 5);
    } else if (cell_type == VTK_HEXAHEDRON) {
        assert(faces.size() == 6);
    } else if (cell_type == VTK_TETRA) {
        assert(faces.size() == 4);
    }
    return faces;
}

namespace std {
    template<>
    struct hash<std::array<vtkIdType, 3>> {
        // https://wjngkoh.wordpress.com/2015/03/04/c-hash-function-for-eigen-matrix-and-vector/
        size_t operator()(const std::array<vtkIdType, 3> &v) const {
            size_t seed = 0;
            for (int i = 0; i < 3; i++) {
                seed ^= std::hash<vtkIdType>()(v[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

int main(int argc, char *argv[]) {
    // check arguments
    if (argc != 5) {
        spdlog::error("Usage: {} <original surface mesh> <volume mesh> <vertex index vector> <output>", argv[0]);
        return 1;
    }

    // read surface mesh(vtk)
    vtkNew<vtkGenericDataObjectReader> reader;
    reader->SetFileName(argv[1]);
    reader->Update();
    if (!reader->IsFileUnstructuredGrid()) {
        throw std::runtime_error("File is not a vtkUnstructuredGrid");
    }
    auto *surface_ugrid = reader->GetUnstructuredGridOutput();
    auto surface_cell_data = surface_ugrid->GetCellData()->GetArray(0);
    if (surface_cell_data == nullptr) {
        throw std::runtime_error("surface mesh has no cell data (or the cell data is not an integer array)");
    }
    int num_surface_points = surface_ugrid->GetNumberOfPoints();
    SPDLOG_INFO("surface points: {}", num_surface_points);
    SPDLOG_INFO("surface cells: {}", surface_ugrid->GetNumberOfCells());

    // construct unordered map: sorted cell points vector (i, j, k) -> cell data (label)
    std::unordered_map<std::array<vtkIdType, 3>, int> label_map;
    for (vtkIdType i = 0; i < surface_ugrid->GetNumberOfCells(); ++i) {
        auto cell = surface_ugrid->GetCell(i);
        auto ids = cell->GetPointIds();
        std::array f = {ids->GetId(0), ids->GetId(1), ids->GetId(2)};
        std::sort(f.begin(), f.end());
        label_map.emplace(f, int(surface_cell_data->GetTuple1(i)));
    }

    // read volume mesh(vtk)
    reader->SetFileName(argv[2]);
    reader->Update();
    if (!reader->IsFileUnstructuredGrid()) {
        throw std::runtime_error("File is not a vtkUnstructuredGrid");
    }
    auto *volume_ugrid = reader->GetUnstructuredGridOutput();
    auto volume_points = volume_ugrid->GetPoints();
    SPDLOG_INFO("volume points: {}", volume_ugrid->GetNumberOfPoints());
    SPDLOG_INFO("volume cells: {}", volume_ugrid->GetNumberOfCells());

    // read vertex index vector B
    std::ifstream fin(argv[3]);
    if (!fin.good()) {
        throw std::runtime_error(std::string(argv[3]) + " cannot be opened");
    }

    // construct index vector
    auto B = index_vector(fin);

    vtkNew<vtkUnstructuredGrid> ugrid_new;
    ugrid_new->SetPoints(volume_points);
    vtkNew<vtkIntArray> scalar;
    scalar->SetName("face_label");
    ugrid_new->GetCellData()->AddArray(scalar);

    // iterate all cells of volume mesh
    for (vtkIdType i = 0; i < volume_ugrid->GetNumberOfCells(); ++i) {
        auto cell = volume_ugrid->GetCell(i);
        // extract faces from cell
        auto faces = get_faces(cell);
        for (const auto &face: faces) {
            std::vector<vtkIdType> S;
            // for vertex (id:i), B[i] -> (i1, i2, i3)
            // set S = merge{B[i], B[j], B[k], B[l]}
            for (vtkIdType vid: face) {
                std::copy(B[vid].begin(), B[vid].end(), std::back_inserter(S));
            }
            std::sort(S.begin(), S.end());
            auto last = std::unique(S.begin(), S.end());
            S.erase(last, S.end());

            if (S.size() != 3)
                continue;
            if (S[0] >= num_surface_points || S[1] >= num_surface_points || S[2] >= num_surface_points)
                continue;
            // if S == {i, j, k} and i, j, k < #v
            std::array set{S[0], S[1], S[2]};
            auto result = label_map.find(set);
            if (result != label_map.end()) {
                // add face
                if (face.size() == 3) {
                    // find face f' = {i, j, k} from original mesh (sort S and faces from original mesh)
                    // L[f] = L'[f']
                    ugrid_new->InsertNextCell(VTK_TRIANGLE, 3, face.data());
                    scalar->InsertNextValue(result->second);
                } else if (face.size() == 4) {
                    // find face f' = {i, j, k} from original mesh (sort S and faces from original mesh)
                    // L[f] = L'[f']
                    ugrid_new->InsertNextCell(VTK_QUAD, 4, face.data());
                    scalar->InsertNextValue(result->second);
                }
            }
        }
    }

    // output
    vtkNew<vtkUnstructuredGridWriter> writer;
    writer->SetFileVersion(42);
    writer->SetInputData(ugrid_new);
    writer->SetFileName(argv[4]);
    writer->Write();
}