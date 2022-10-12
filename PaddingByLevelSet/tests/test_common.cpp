#include <doctest/doctest.h>

#include <lspadding/common.hpp>

TEST_CASE("test sparse vector equality") {
    Eigen::SparseVector<double> a(10), b(10);
    a.coeffRef(3) = 1.0;
    b.coeffRef(3) = 1.0;

    lspadding::EqualSV<lspadding::rint> equal;
    CHECK(equal(a, b));

    b.coeffRef(3) = 2.0;
    CHECK(!equal(a, b));
}