#pragma once
#include <3rd/rationals.hpp>

#include <variant>
#include <type_traits>
#include <Eigen/Core>
#include <Eigen/Sparse>

#define NOT_IMPLEMENTED static_assert(false, "Not implemented")

namespace lspadding {
template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
Overload(Ts...) -> Overload<Ts...>;

template <class T, class TypeList>
struct IsContainedIn;

template <class T, class... Ts>
struct IsContainedIn<T, std::variant<Ts...>>
    : std::bool_constant<(... || std::is_same<T, Ts>{})> {};

template <typename T, typename V>
concept ContainedIn = IsContainedIn<T, V>::value;

using rint = rationals::rational<int>;

} // namespace lspadding

namespace Eigen {
template <>
struct NumTraits<lspadding::rint> : GenericNumTraits<lspadding::rint> {
    typedef lspadding::rint Real;
    typedef lspadding::rint NonInteger;
    typedef lspadding::rint Nested;

    static inline Real epsilon() { return 0; }
    static inline Real dummy_precision() { return 0; }
    static inline int digits10() { return 0; }

    enum {
        IsInteger             = 0,
        IsSigned              = 1,
        IsComplex             = 0,
        RequireInitialization = 1,
        ReadCost              = 6,
        AddCost               = 150,
        MulCost               = 100
    };
};

} // namespace Eigen

namespace std {
template <typename Scalar>
struct hash<Eigen::SparseVector<Scalar>> {
    // https://wjngkoh.wordpress.com/2015/03/04/c-hash-function-for-eigen-matrix-and-vector/
    size_t operator()(const Eigen::SparseVector<Scalar> &matrix) const {
        size_t seed = 0;

        using iter = typename Eigen::SparseVector<Scalar>::InnerIterator;
        for (iter it(matrix); it; ++it) {
            seed ^= std::hash<Scalar>()(it.value()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= std::hash<Scalar>()(it.index()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
template <>
struct hash<lspadding::rint> {
    size_t operator()(const lspadding::rint &r) const {
        return std::hash<int>()(r.numerator()) ^ std::hash<int>()(r.denominator());
    }
};

} // namespace std

namespace lspadding {
template <typename Scalar>
struct EqualSV {
    bool operator()(const Eigen::SparseVector<Scalar> &a, const Eigen::SparseVector<Scalar> &b) const {

        if (a.size() != b.size() || a.innerSize() != b.innerSize()) {
            return false;
        }
        using iter = typename Eigen::SparseVector<Scalar>::InnerIterator;
        for (iter it1(a), it2(b); it1 && it2; ++it1, ++it2) {
            if (it1.value() != it2.value() || it1.index() != it2.index()) {
                return false;
            }
        }
        return true;
    }
};
} // namespace lspadding