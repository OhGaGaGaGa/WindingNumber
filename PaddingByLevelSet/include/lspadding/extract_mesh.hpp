#pragma once

#include <functional>
#include <stdexcept>
#include <unordered_map>

#include "celltypes.hpp"
#include "common.hpp"
#include "vtkCellType.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkType.h"
#include <Eigen/Sparse>

#include <vtkNew.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>

#include <spdlog/spdlog.h>
#include <fmt/ranges.h>
#include <fmt/ostream.h>

namespace lspadding::extract {
template <typename T>
[[nodiscard]] inline constexpr VTKCellType vtk_cell_type();
template <>
[[nodiscard]] inline constexpr VTKCellType vtk_cell_type<Tet>() {
    return VTK_TETRA;
}
template <>
[[nodiscard]] inline constexpr VTKCellType vtk_cell_type<Hex>() {
    return VTK_HEXAHEDRON;
}
template <>
[[nodiscard]] inline constexpr VTKCellType vtk_cell_type<Wedge>() {
    return VTK_WEDGE;
}
template <>
[[nodiscard]] inline constexpr VTKCellType vtk_cell_type<Pyramid>() {
    return VTK_PYRAMID;
}
struct ExtractContext {
    Eigen::VectorX<rint> scalar_field;
    const Eigen::MatrixX<double> &pos;

    ExtractContext(
        const Eigen::VectorXi &scalar_field,
        const Eigen::MatrixX<double> &pos) : scalar_field(scalar_field.cast<rint>()), pos(pos) {}
    using umap = std::unordered_map<
        Vertex, int,
        std::hash<Vertex>,
        EqualSV<rint>,
        Eigen::aligned_allocator<std::pair<const Vertex, int>>>;
    void add_vertices(umap &vertex_map, std::vector<Vertex> &vertices, const std::vector<CellDone> &cells) const {
        const Eigen::SparseVector<rint> a, b;
        for (auto &&cell : cells) {
            std::visit(
                [&](auto &&cell) {
                    for (const Vertex &v : cell.vertices) {
                        if (!vertex_map.contains(v)) {
                            vertex_map[v] = vertex_map.size();
                            vertices.push_back(v);
                        }
                    }
                },
                cell);
        }
    }
    template <bool save_scalar_field = false>
    std::tuple<vtkSmartPointer<vtkUnstructuredGrid>, std::vector<Vertex>> extract(const std::vector<CellDone> &cells) {
        umap vertex_map;
        std::vector<Vertex> vertices;
        vtkNew<vtkUnstructuredGrid> ugrid;
        vtkNew<vtkPoints> points;

        add_vertices(vertex_map, vertices, cells);
        points->SetNumberOfPoints(vertices.size());
        vtkNew<vtkIntArray> scalar;
        if constexpr (save_scalar_field) {
            scalar->Allocate(vertices.size());
            scalar->SetNumberOfValues(vertices.size());
            scalar->SetName("padding");
            ugrid->GetPointData()->AddArray(scalar);
        }

#pragma omp parallel for
        for (int i = 0; i < vertices.size(); i++) {
            auto &&v = vertices[i];
            auto dv  = v.unaryExpr([](auto &&x) { return (double)x.numerator() / x.denominator(); });

            Eigen::Vector3d p = dv.transpose() * pos;
            points->SetPoint(i, p.data());
            if constexpr (save_scalar_field) {
                rint s = v.dot(scalar_field);
                scalar->SetValue(i, s.numerator());
            }
        }

        ugrid->SetPoints(points);

#pragma omp parallel for
        for (auto &&cell : cells) {
            std::visit(
                Overload{
                    [&](const WedgeN &wn) {
                        throw std::runtime_error("not implemented");
                    },
                    [&](const WedgeS &ws) {
                        throw std::runtime_error("not implemented");
                    },
                    [&](auto &&cell) {
                        using T = std::decay_t<decltype(cell)>;
                        std::array<vtkIdType, T::num_v> ids{};
                        for (int i = 0; i < T::num_v; i++) {
                            ids[i] = vertex_map[cell.vertices[i]];
                        }
#pragma omp critical
                        ugrid->InsertNextCell(vtk_cell_type<T>(), T::num_v, ids.data());
                    },
                },
                cell);
        }
        return {ugrid, std::move(vertices)};
    }
    template <bool legacy = false>
    [[nodiscard]] std::string output(vtkSmartPointer<vtkUnstructuredGrid> ugrid) const {
        vtkNew<vtkUnstructuredGridWriter> writer;
        if constexpr (legacy) {
            writer->SetFileVersion(42);
        }
        writer->SetInputData(ugrid);
        writer->WriteToOutputStringOn();
        writer->Write();

        std::string ret = writer->GetOutputString();

        return ret;
    }

    [[nodiscard]] std::string output(const std::vector<Vertex> &vertices) const {
        std::string ret;
        for (const auto &vertex : vertices) {
            for (Vertex::InnerIterator it(vertex); it; ++it) {
                ret += std::to_string(it.index()) + " ";
            }
            ret += "\n";
        }

        return ret;
    }
};
} // namespace lspadding::extract