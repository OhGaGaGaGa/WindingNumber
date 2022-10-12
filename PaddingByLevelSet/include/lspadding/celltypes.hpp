#pragma once

#include <array>
#include <tuple>
#include <variant>

#include "common.hpp"
#include <Eigen/Sparse>

namespace lspadding {

using Vertex = Eigen::SparseVector<rint>;

inline Vertex vertex_at(int x, int n) {
    Eigen::SparseVector<rint> v(n);
    v.setZero();
    v.coeffRef(x) = rint(1);
    return v;
}

template <std::size_t V>
struct CellBase {
    constexpr static auto num_v = V;
    std::array<Vertex, num_v> vertices;
};
template <std::size_t V, typename Tag>
struct TaggedCell : public CellBase<V> {
    template <typename... Ts>
    static constexpr TaggedCell<V, Tag> make(Ts &&...vs) noexcept(std::is_nothrow_constructible_v<Vertex, Ts...>) {
        TaggedCell ret;
        ret.vertices = std::array{std::forward<Ts>(vs)...};
        return ret;
    }
};

enum class CutPattern {
    T13,
    T22,
    T31,
    T112,
    T121,
    T211,
    T1111,
    W33,
    W1212,
    W2121,
    P311,
    P32,
    P23,
    WN,
    WS
};
#define MAKE_CELL(NAME, n) \
    struct NAME##Tag;      \
    using NAME = TaggedCell<n, NAME##Tag>

MAKE_CELL(Tet13, 4);
MAKE_CELL(Tet22, 4);
MAKE_CELL(Tet31, 4);
MAKE_CELL(Tet112, 4);
MAKE_CELL(Tet121, 4);
MAKE_CELL(Tet211, 4);
MAKE_CELL(Tet1111, 4);
MAKE_CELL(Wedge33, 6);
MAKE_CELL(Wedge1212, 6);
MAKE_CELL(Wedge2121, 6);
MAKE_CELL(Pyramid311, 5);
MAKE_CELL(Pyramid32, 5);
MAKE_CELL(Pyramid23, 5);
MAKE_CELL(WedgeN, 6);
MAKE_CELL(WedgeS, 7);

MAKE_CELL(Tet, 4);
MAKE_CELL(Wedge, 6);
MAKE_CELL(Pyramid, 5);
MAKE_CELL(Hex, 8);

#undef MAKE_CELL

using Cell      = std::variant<Tet, Hex, Wedge, Pyramid, WedgeN, WedgeS,
                          Tet13, Tet22, Tet31, Tet112, Tet121, Tet211, Tet1111,
                          Wedge33, Wedge1212, Wedge2121, Pyramid311, Pyramid32, Pyramid23>;
using CellToCut = std::variant<Tet13, Tet22, Tet31, Tet112, Tet121, Tet211, Tet1111,
                               Wedge33, Wedge1212, Wedge2121, Pyramid311, Pyramid32, Pyramid23,
                               WedgeN, WedgeS>;

using CellDone = std::variant<Tet, Wedge, Hex, Pyramid, WedgeN, WedgeS>;
} // namespace lspadding
