#pragma once

#include <cassert>

#include "celltypes.hpp"
#include "common.hpp"

#include <Eigen/Sparse>
namespace lspadding::cuts {

inline Vertex lerp(const Vertex &a, const Vertex &b, rint x) {
    return a * (rint(1) - x) + b * x;
}

inline rint levelset(int a, int b, int fx) {
    return rint(fx - a) / rint(b - a);
}
template <typename T>
struct SedCell {
    const T &cell;
    std::array<int, T::num_v> scalar_at_vertices;

    [[nodiscard]] int scalar_at(int i) const {
        return scalar_at_vertices[i];
    }

    [[nodiscard]] Vertex left_inc(int left, int right, int inc) const {
        auto x = levelset(scalar_at(left), scalar_at(right), scalar_at(left) + inc);
        return lerp(cell.vertices[left], cell.vertices[right], x);
    }

    [[nodiscard]] Vertex right_dec(int left, int right, int dec) const {
        auto x = levelset(scalar_at(left), scalar_at(right), scalar_at(right) - dec);
        return lerp(cell.vertices[left], cell.vertices[right], x);
    }

    [[nodiscard]] Vertex insert_at(int left, int right, int scalar) const {
        auto x = levelset(scalar_at(left), scalar_at(right), scalar);
        return lerp(cell.vertices[left], cell.vertices[right], x);
    }

    [[nodiscard]] const Vertex &vert(int i) const {
        return cell.vertices[i];
    }
};
using CellPair = std::tuple<Cell, Cell>;

inline CellPair t13( // 1
    const SedCell<Tet13> &tet) {

    auto &&vf = tet.vert(0);

    auto &&vfk1_1 = tet.vert(1),
         &&vfk1_2 = tet.vert(2),
         &&vfk1_3 = tet.vert(3);

    auto vfk_1 = tet.right_dec(0, 1, 1),
         vfk_2 = tet.right_dec(0, 2, 1),
         vfk_3 = tet.right_dec(0, 3, 1);
    return {
        Tet13::make(vf, vfk_1, vfk_2, vfk_3),
        Wedge33::make(vfk_1, vfk_2, vfk_3, vfk1_1, vfk1_2, vfk1_3)};
}

inline CellPair t22( // 2
    const SedCell<Tet22> &tet) {
    auto &&vf_1 = tet.vert(0), &&vf_2 = tet.vert(1);
    auto vfk1_1 = tet.vert(2), vfk1_2 = tet.vert(3);

    auto vfk_1 = tet.right_dec(0, 2, 1), vfk_2 = tet.right_dec(0, 3, 1);
    auto vfk_3 = tet.right_dec(1, 2, 1), vfk_4 = tet.right_dec(1, 3, 1);

    return {
        Wedge1212::make(vf_1, vfk_2, vfk_1, vf_2, vfk_4, vfk_3),
        Wedge2121::make(vfk_2, vfk_4, vfk1_2, vfk_1, vfk_3, vfk1_1)};
}

inline CellPair t31( // 3
    const SedCell<Tet31> &tet) {

    auto &&vf_1 = tet.vert(0), &&vf_2 = tet.vert(1), &&vf_3 = tet.vert(2);

    auto &&vfk1 = tet.vert(3);

    auto vfk_1 = tet.right_dec(0, 3, 1),
         vfk_2 = tet.right_dec(1, 3, 1),
         vfk_3 = tet.right_dec(2, 3, 1);
    return {
        Tet31::make(vfk_1, vfk_2, vfk_3, vfk1),
        Wedge33::make(vf_1, vf_2, vf_3, vfk_1, vfk_2, vfk_3)};
}
inline CellPair t112( // 4
    const SedCell<Tet112> &tet) {

    auto &&vf     = tet.vert(0);
    auto &&vfk_1  = tet.vert(1);
    auto &&vfkg_1 = tet.vert(2), &&vfkg_2 = tet.vert(3);

    auto fk    = tet.scalar_at(1);
    auto vfk_2 = tet.insert_at(0, 2, fk), vfk_3 = tet.insert_at(0, 3, fk);

    return {
        Tet13::make(vf, vfk_2, vfk_3, vfk_1),
        Pyramid32::make(vfk_1, vfk_2, vfk_3, vfkg_2, vfkg_1)};
}

inline CellPair w33( // 8
    const SedCell<Wedge33> &wedge) {

    auto &&vf_1 = wedge.vert(0), &&vf_2 = wedge.vert(1), &&vf_3 = wedge.vert(2);
    auto &&vfk1_1 = wedge.vert(3), &&vfk1_2 = wedge.vert(4), &&vfk1_3 = wedge.vert(5);

    auto vfk_1 = wedge.right_dec(0, 3, 1),
         vfk_2 = wedge.right_dec(1, 4, 1),
         vfk_3 = wedge.right_dec(2, 5, 1);
    return {
        Wedge33::make(vf_1, vf_2, vf_3, vfk_1, vfk_2, vfk_3),
        Wedge33::make(vfk_1, vfk_2, vfk_3, vfk1_1, vfk1_2, vfk1_3)};
}

inline CellPair w1212( // 9
    const SedCell<Wedge1212> &wedge) {
    auto &&vf_1 = wedge.vert(0), &&vf_2 = wedge.vert(3);
    auto &&vfk1_1 = wedge.vert(1), &&vfk1_2 = wedge.vert(2), &&vfk1_3 = wedge.vert(4), &&vfk1_4 = wedge.vert(5);

    auto vfk_1 = wedge.right_dec(0, 1, 1),
         vfk_2 = wedge.right_dec(0, 2, 1),
         vfk_3 = wedge.right_dec(3, 4, 1),
         vfk_4 = wedge.right_dec(3, 5, 1);
    return {
        Wedge1212::make(vf_1, vfk_1, vfk_2, vf_2, vfk_3, vfk_4),
        Hex::make(vfk_1, vfk_2, vfk_4, vfk_3, vfk1_1, vfk1_2, vfk1_4, vfk1_3)};
}

} // namespace lspadding::cuts