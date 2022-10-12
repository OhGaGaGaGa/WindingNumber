#include "lspadding/celltypes.hpp"
#include "lspadding/extract_mesh.hpp"

#include <doctest/doctest.h>

using namespace lspadding;

TEST_CASE("test extract") {
    Eigen::VectorXi scalar_field{{0, 2, 2, 2}};
    Eigen::MatrixXd pos{
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0}};
    lspadding::extract::ExtractContext context(scalar_field, pos);
    std::vector<lspadding::CellDone> tets{Tet{vertex_at(0, 4),
                                              vertex_at(1, 4),
                                              vertex_at(2, 4),
                                              vertex_at(3, 4)}};
    auto [grid, vertices_vector] = context.extract(tets);

    for (const auto &vertex : vertices_vector) {
        CHECK(vertex.nonZeros() == 1);
    }
    CHECK(grid->GetNumberOfPoints() == 4);
    CHECK(context.output(vertices_vector)=="0 \n1 \n2 \n3 \n");
    CHECK(context.output(grid) == R"(# vtk DataFile Version 5.1
vtk output
ASCII
DATASET UNSTRUCTURED_GRID
POINTS 4 float
0 0 0 0 0 0 0 0 0 
0 0 0 
CELLS 2 4
OFFSETS vtktypeint64
0 4 
CONNECTIVITY vtktypeint64
0 1 2 3 
CELL_TYPES 1
10

)");
}
