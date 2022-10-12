#ifndef MESH_TOOLS_TETRAHEDRALIZE_H
#define MESH_TOOLS_TETRAHEDRALIZE_H

#include "tetgen.h"
#include "spdlog/spdlog.h"

#include "mesh.hpp"

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMeshTopologyKernel.hh>

using Vec3d   = OpenVolumeMesh::Geometry::Vec3d;
using TetMesh = OpenVolumeMesh::GeometryKernel<Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel>;

namespace mesh_tools {
TetMesh Tetrahedralize(const Eigen::Matrix3Xd &V,
                       const Eigen::Matrix3Xi &F);

// ===== Implement =====
tetgenio ConvertMeshToIn(const Eigen::Matrix3Xd &V,
                         const Eigen::Matrix3Xi &F) {
    tetgenio in;
    in.firstnumber     = 0;
    in.numberofpoints  = (int)V.cols();
    in.pointlist       = new REAL[in.numberofpoints * 3];
    in.pointmarkerlist = new int[in.numberofpoints];
    for (int i = 0; i < in.numberofpoints; ++i) {
        try {
            in.pointlist[i * 3]     = V(0, i);
            in.pointlist[i * 3 + 1] = V(1, i);
            in.pointlist[i * 3 + 2] = V(2, i);
        } catch (const char *msg) {
            SPDLOG_ERROR("{}", msg);
        }
    }
    in.numberoffacets = (int)F.cols();
    in.facetlist      = new tetgenio::facet[in.numberoffacets];
    for (int i = 0; i < in.numberoffacets; ++i) {
        tetgenio::facet *f   = &in.facetlist[i];
        f->numberofpolygons  = 1;
        f->polygonlist       = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes     = 0;
        f->holelist          = nullptr;
        tetgenio::polygon *p = &f->polygonlist[0];
        p->numberofvertices  = 3;
        p->vertexlist        = new int[p->numberofvertices];
        p->vertexlist[0]     = F(0, i);
        p->vertexlist[1]     = F(1, i);
        p->vertexlist[2]     = F(2, i);
    }
    return in;
}

TetMesh ConvertOutToMesh(const tetgenio &out) {
    TetMesh volume_mesh;
    assert(out.pointlist != nullptr);
    std::vector<OpenVolumeMesh::VertexHandle> vhandles;
    for (int i = 0; i < out.numberofpoints; i++) {
        double x = out.pointlist[3 * i + 0];
        double y = out.pointlist[3 * i + 1];
        double z = out.pointlist[3 * i + 2];
        vhandles.push_back(volume_mesh.add_vertex({x, y, z}));
    }
    assert(out.tetrahedronlist != nullptr);
    assert(out.numberofcorners == 4);
    for (int i = 0; i < out.numberoftetrahedra; i++) {
        int offset = i * out.numberofcorners;
        volume_mesh.add_cell({vhandles[out.tetrahedronlist[offset + 0]],
                              vhandles[out.tetrahedronlist[offset + 1]],
                              vhandles[out.tetrahedronlist[offset + 2]],
                              vhandles[out.tetrahedronlist[offset + 3]]});
    }
    assert(out.trifacemarkerlist != nullptr);
    SPDLOG_INFO("number of points in output: {}", out.numberofpoints);
    // print out the marker list
    SPDLOG_DEBUG("out.numberoftrifaces {}", out.numberoftrifaces);

    return volume_mesh;
}

TetMesh Tetrahedralize(const Eigen::Matrix3Xd &V,
                       const Eigen::Matrix3Xi &F,
                       double max_length) {
    tetgenbehavior b;
    b.plc              = true; // '-p', 0.
    b.nobisect         = true; // '-Y', 0. keep the original surface in the mesh.
    b.quality          = true; // '-q', 0. insert some vertices to ensure the quality.
    b.varvolume        = true;
    b.fixedvolume      = true;
    b.maxvolume        = 0.1178511301978 * max_length * max_length * max_length; // sqrt(2) / 12 * a ^ 3
    b.maxvolume_length = max_length;
    tetgenio in        = ConvertMeshToIn(V, F);
    tetgenio out;
    SPDLOG_INFO("Tetrahedralize...");
    tetrahedralize(&b, &in, &out);
    SPDLOG_INFO("Tetrahedralize finished.");
    return ConvertOutToMesh(out);
}
}; // namespace mesh_tools

#endif