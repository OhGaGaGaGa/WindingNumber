//
// Created by ybc on 22-7-4.
//

#ifndef TET_LABELING_REMOVE_OUTSIDE_CELLS_HPP
#define TET_LABELING_REMOVE_OUTSIDE_CELLS_HPP

#include <string>
#include <random>

#include <Eigen/Dense>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMeshTopologyKernel.hh>
#include "config.hpp"

using Vec3d = OpenVolumeMesh::Geometry::Vec3d;
using SurfaceMesh = OpenMesh::TriMesh_ArrayKernelT<>;
using TetMesh = OpenVolumeMesh::GeometryKernel<Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>;

namespace std {
    template<>
    struct hash<std::array<int, 3>> {
        // https://wjngkoh.wordpress.com/2015/03/04/c-hash-function-for-eigen-matrix-and-vector/
        size_t operator()(const std::array<int, 3> &v) const {
            std::size_t h = 0;
            for (auto e: v) {
                h ^= std::hash<int>{}(e) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}

namespace tetlabel {
    std::unordered_map<std::array<int, 3>, int> MarkConnectedComponents(const SurfaceMesh &mesh) {
        SPDLOG_INFO("Mark connected components...");
        auto visited = OpenMesh::FProp<bool>(mesh);
        auto components = OpenMesh::FProp<int>(mesh);
        for (auto f: mesh.faces()) {
            visited[f] = false;
            assert(components[f] == 0);
        }

        int component_id = 1;
        for (auto f: mesh.faces()) {
            if (!visited[f]) {
                visited[f] = true;
                std::queue<decltype(f)> queue;
                queue.push(f);
                components[f] = component_id;
                while (!queue.empty()) {
                    auto queue_f = queue.front();
                    queue.pop();
                    for (auto ff: queue_f.faces()) {
                        if (!visited[ff]) {
                            visited[ff] = true;
                            queue.push(ff);
                            components[ff] = component_id;
                        }
                    }
                }
                ++component_id;
            }
        }

        std::unordered_map<std::array<int, 3>, int> face_component_id_map;
        for (auto f: mesh.faces()) {
            std::vector<int> face_indices;
            for (auto v: f.vertices()) {
                face_indices.emplace_back(v.idx());
            }
            std::sort(face_indices.begin(), face_indices.end());
            face_component_id_map[{face_indices[0], face_indices[1], face_indices[2]}] = components[f];
        }
        return face_component_id_map;
    }

    // ! it only works if the tet mesh has only a single connected component
    void RemoveOutsideCells(TetMesh &tet_mesh, const std::unordered_map<std::array<int, 3>, int> &face_id_map) {
        SPDLOG_INFO("Remove cells out of the given boundary...");
        OpenVolumeMesh::FacePropertyT<int> face_label =
                tet_mesh.request_face_property<int>("face_component_id");
        // initialize the label
        for (auto f: tet_mesh.faces()) {
            assert(face_label[f] == 0);
        }

        for (auto f: tet_mesh.faces()) {
            std::vector<int> face_indices;
            for (auto v: tet_mesh.face_vertices(f)) {
                face_indices.push_back(v.idx());
            }
            std::sort(face_indices.begin(), face_indices.end());
            auto face_id = face_id_map.find({face_indices[0], face_indices[1], face_indices[2]});
            if(face_id != face_id_map.end()) {
                face_label[f] = face_id->second;
            }
        }

        SPDLOG_DEBUG("Start traversing the mesh...");
        OpenVolumeMesh::CellPropertyT<bool> visited_cell =
                tet_mesh.request_cell_property<bool>("visited_cell");
        OpenVolumeMesh::FacePropertyT<bool> visited_face =
                tet_mesh.request_face_property<bool>("visited_face");

        // start from any exterior boundary cell
        // BFS: mark each touched cell
        // find the first boundary face
        int boundary_component_id = 0;
        auto c0 = *tet_mesh.bc_iter();
        for (auto cf: tet_mesh.cell_faces(c0)) {
            if (face_label[cf] != 0) {
                assert(tet_mesh.is_boundary(cf));
                boundary_component_id = face_label[cf];
                break;
            }
        }
        visited_cell[c0] = true;
        assert(boundary_component_id != 0);
        std::queue<decltype(c0)> queue;
        queue.push(c0);
        while (!queue.empty()) {
            auto c = queue.front();
            queue.pop();
            for (auto cf: tet_mesh.cell_faces(c)) {
                if (!visited_face[cf]) {
                    visited_face[cf] = true;
                    if (face_label[cf] == boundary_component_id) {
                        // boundary
                        assert(tet_mesh.is_boundary(cf));
                    }
                    if (face_label[cf] == 0) {
                        // still not touch the interior boundary, add the opposite cell to the queue
                        auto opp_c = tet_mesh.incident_cell(cf.halfface_handle(0));
                        if (opp_c == c) {
                            opp_c = tet_mesh.incident_cell(cf.halfface_handle(1));
                        }
                        if (opp_c.is_valid()) {
                            assert (opp_c != c);
                            visited_cell[opp_c] = true;
                            queue.push(opp_c);
                        } else {
                            SPDLOG_DEBUG("opp_c is invalid");
                        }
                    }
                }
            }
        }

        SPDLOG_DEBUG("Remove cells that are not touched...");
        // remove all cells that are not touched
        for (auto c: tet_mesh.cells()) {
            if (!visited_cell[c]) {
                tet_mesh.delete_cell(c);
            }
        }
        tet_mesh.collect_garbage();
    }

} // namespace tetlabel

#endif //TET_LABELING_REMOVE_OUTSIDE_CELLS_HPP
