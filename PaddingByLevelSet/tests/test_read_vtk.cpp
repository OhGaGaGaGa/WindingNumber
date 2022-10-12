#include <doctest/doctest.h>
#include <lspadding/read_vtk.hpp>

#include <source_location>
#include <filesystem>

#include "test.hpp"

TEST_CASE("test read_vtk") {
    SUBCASE("correct file") {
        std::string filename = LS_TEST_DIR + "/data/cube3x3.vtk";

        auto [scalars, vertices, tets] = lspadding::read_vtk(filename);

        CHECK(vertices.rows() == 91);
        CHECK(scalars.rows() == 91);
        CHECK(tets.size() == 324);

        for (int i = 0; i < scalars.rows(); i++) {
            if (i < 10) {
                CHECK(scalars(i) == 3);
            } else {
                CHECK(scalars(i) == 0);
            }
        }
        for (auto &&tet : tets) {
            for (auto &&v : tet.vertices) {
                CHECK(v.rows() == vertices.rows());
            }
        }
    }
    SUBCASE("file without padding scalar") {
        std::string current_file = __FILE__;
        std::string current_dir  = std::filesystem::path(current_file).parent_path().string();

        CHECK(current_dir.ends_with("tests"));

        std::string filename = current_dir + "/data/broken/cube3x3_no_padding.vtk";

        CHECK_THROWS_WITH(lspadding::read_vtk(filename), "File does not contain padding info");
    }
}