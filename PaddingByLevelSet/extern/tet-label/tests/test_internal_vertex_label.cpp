#include "doctest/doctest.h"
#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
// #include "vtk_io.h"

typedef OpenVolumeMesh::Geometry::Vec3d Vec3d;
using TetMesh = OpenVolumeMesh::GeometryKernel<Vec3d, OpenVolumeMesh::TetrahedralMeshTopologyKernel> ;
TEST_CASE("Test internal vertex label") {
    TetMesh tet_mesh;
    // OpenVolumeMesh::IO::FileManager fileManager;
    // read_vtk_with_cell_data()
    // fileManager.readFile("../../data/out.vtk", tet_mesh);
    tet_mesh.add_vertex({0,0,0});
    CHECK (tet_mesh.n_vertices() != 0);
}
