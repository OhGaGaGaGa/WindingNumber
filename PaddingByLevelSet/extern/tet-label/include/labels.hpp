//
// Created by ybc on 22-7-4.
//

#ifndef TET_LABELING_LABELS_HPP
#define TET_LABELING_LABELS_HPP

#include <string>
#include <random>

#include <Eigen/Dense>
#include "config.hpp"
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

namespace tetlabel {

    std::vector<int> ExtractBoundaryLayersInfo(const Eigen::Matrix3Xi &F,
                                               const Eigen::VectorXi &face_labels,
                                               const Config &config) {
        std::map<int, int> label_to_layer;
        for (const ConfigItem &item: config.items) {
            label_to_layer[item.label] = item.layers;
        }
        std::vector<int> layers(F.cols(), 0);
        for (int i = 0; i < F.cols(); ++i) {
            layers[i] = label_to_layer[face_labels[i]];
        }
        return layers;
    }

    std::vector<int> RandomLayersInfo(int num_vertices) {
        std::vector<int> boundary_layers(num_vertices, 0);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(5, 15);
        for (int i = 0; i < num_vertices; ++i) {
            boundary_layers[i] = dis(gen);
        }
        return boundary_layers;
    }


// interpolate the labels at cells to vertices
    std::vector<int> Interpolate(const OpenMesh::TriMesh_ArrayKernelT<> &mesh, const std::vector<int> &label_at_cells) {
        std::vector<int> label_at_vertices(mesh.n_vertices(), 0);
        for (const auto &v: mesh.vertices()) {
            int label = 0;
            // max of all labels at the vertices
            for (const auto &f: v.faces()) {
                label = std::max(label, label_at_cells[f.idx()]);
            }
            label_at_vertices[v.idx()] = label;
        }
        return label_at_vertices;
    }

} // namespace tetlabel

#endif //TET_LABELING_LABELS_HPP
