//
// Created by ybc on 22-7-4.
//

#ifndef TET_LABELING_FORMAT_CONVERTER_HPP
#define TET_LABELING_FORMAT_CONVERTER_HPP

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMeshTopologyKernel.hh>
#include <Eigen/Dense>

#include "mesh.hpp"

using SurfaceMesh = OpenMesh::TriMesh_ArrayKernelT<>;
using TetMesh = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Geometry::Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>;

namespace tetlabel {
    template<typename Point>
    mesh_tools::mesh::Mesh<Point> Convert(const TetMesh &tet_mesh) {
        SPDLOG_INFO("Convert TetMesh to mesh_tools::mesh::Mesh...");
        mesh_tools::mesh::Mesh<OpenVolumeMesh::Geometry::Vec3d> result;
        for (auto v: tet_mesh.vertices()) {
            result.AddVertex(tet_mesh.vertex(v));
        }
        for (auto c: tet_mesh.cells()) {
            std::vector<std::size_t> vertices;
            for (auto v: tet_mesh.cell_vertices(c)) {
                vertices.push_back(v.idx());
            }
            result.AddCell(mesh_tools::mesh::Cell(mesh_tools::mesh::TET, vertices));
        }
        return result;
    }

    SurfaceMesh Convert(Eigen::Matrix3Xd V, Eigen::Matrix3Xi F) {
        SPDLOG_INFO("Convert mesh_tools::mesh::Mesh to SurfaceMesh...");
        SurfaceMesh surface_mesh;
        std::vector<SurfaceMesh::VertexHandle> vertices;
        for (int i = 0; i < V.cols(); ++i) {
            vertices.push_back(surface_mesh.add_vertex({V(0, i), V(1, i), V(2, i)}));
        }
        for (int i = 0; i < F.cols(); ++i) {
            surface_mesh.add_face(vertices[F(0, i)], vertices[F(1, i)], vertices[F(2, i)]);
        }
        return surface_mesh;
    }
}
#endif //TET_LABELING_FORMAT_CONVERTER_HPP
