#include "lspadding/celltypes.hpp"
#include "lspadding/cuts.hpp"
#include "doctest/doctest.h"

#include "lspadding/cutting.hpp"

using namespace lspadding;

TEST_CASE("Test Cutting") {
    Eigen::VectorXi scalar_field{{0, 2, 2, 2}};
    lspadding::CutContext context(scalar_field);
    std::vector<Tet> tets{{vertex_at(0, 4),
                           vertex_at(1, 4),
                           vertex_at(2, 4),
                           vertex_at(3, 4)}};

    SUBCASE("normalize tet") {
        auto [pattern, nvertices] = context.normalize(tets[0]);
        CHECK(pattern == std::array{0, 1, 1, 1});
        for (int i = 0; i < 4; i++) {
            CHECK(nvertices[i].size() == 4);
        }
    }

    SUBCASE("terminated t13") {
        auto t13    = Tet13::make(vertex_at(0, 4),
                                  vertex_at(1, 4),
                                  vertex_at(2, 4),
                                  vertex_at(3, 4));
        auto [a, b] = cuts::t13(context.sed_cell(t13));
        CHECK(std::visit([&](auto &&x) { return context.terminate_cell(x); }, b));
    }
    SUBCASE("two result") {
        auto ret = context.cut(tets);

        CHECK(ret.size() == 2);
    }
}