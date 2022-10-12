#include <doctest/doctest.h>

#include "lspadding/cutting.hpp"
#include "lspadding/celltypes.hpp"
#include "lspadding/extract_mesh.hpp"
#include "lspadding/read_vtk.hpp"

#include "test.hpp"

#include <filesystem>
#include <fstream>
#include <omp.h>

TEST_CASE("test integrated") {
    omp_set_num_threads(1);
    CHECK(LS_TEST_DIR.ends_with("tests"));

    std::string filename = LS_TEST_DIR + "/data/cube3x3.vtk";

    auto [scalars, vertices, tets] = lspadding::read_vtk(filename);

    lspadding::CutContext context(scalars);
    auto grid = context.cut(tets);

    lspadding::extract::ExtractContext extract_context(scalars, vertices);
    auto [extracted_cells, vertices_vector] = extract_context.extract(grid);
    auto output                             = extract_context.output(extracted_cells);
    auto output_vertex                      = extract_context.output(vertices_vector);
    for (const auto &vertex : vertices_vector) {
        bool num_zeros = vertex.nonZeros() == 1 || vertex.nonZeros() == 2;
        CHECK(num_zeros);
    }

    std::ifstream fin(LS_TEST_DIR + "/data/cube3x3_padded.vtk"), fin_vertex(LS_TEST_DIR + "/data/vertex_vector.txt");
    std::string answer(std::istreambuf_iterator<char>(fin), {});
    std::string answer_vertex(std::istreambuf_iterator<char>(fin_vertex), {});
    CHECK(answer == output);
    CHECK(answer_vertex == output_vertex);
}