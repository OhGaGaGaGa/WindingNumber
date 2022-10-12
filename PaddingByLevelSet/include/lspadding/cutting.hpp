#pragma once

#include <cmath>
#include <limits>
#include <omp.h>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>
#include <queue>
#include <tuple>
#include <ranges>
#include <algorithm>
#include <iostream>

#include <Eigen/Core>
#include <nameof.hpp>

#include "celltypes.hpp"

#include "common.hpp"
#include "cuts.hpp"
#include <fmt/core.h>

namespace lspadding {
struct CutContext {

    CutContext(const Eigen::VectorXi &scalar_field) : scalar_field(scalar_field.cast<rint>()) {}

    CutContext(CutContext &&)                 = delete;
    CutContext(const CutContext &)            = delete;
    CutContext &operator=(CutContext &&)      = delete;
    CutContext &operator=(const CutContext &) = delete;
    ~CutContext()                             = default;
    template <typename T>
    [[nodiscard]] cuts::SedCell<T> sed_cell(const T &cell) const {
        std::array<int, T::num_v> scalar_at_vertices{};
        for (int i = 0; i < T::num_v; i++) {
            scalar_at_vertices[i] = scalar_at(cell.vertices[i]);
        }
        return cuts::SedCell<T>{cell, std::move(scalar_at_vertices)};
    }
    template <typename T>
    auto make_cut(auto &&cut_func) const {
        return [&](const T &cell) -> cuts::CellPair {
            return cut_func(sed_cell(cell));
        };
    }
    [[nodiscard]] std::tuple<Cell, Cell> cut_one_cell(const CellToCut &cell) const {
        return std::visit(
            Overload{
                make_cut<Tet13>(cuts::t13),
                make_cut<Tet22>(cuts::t22),
                make_cut<Tet31>(cuts::t31),
                make_cut<Wedge33>(cuts::w33),
                make_cut<Wedge1212>(cuts::w1212),
                [](auto &&other) -> cuts::CellPair {
                    using T = std::decay_t<decltype(other)>;
                    static_assert(!std::same_as<T, Tet13>);
                    using namespace std::string_literals;
                    throw std::runtime_error(fmt::format("not implemented for cut: {}", NAMEOF_TYPE(T)));
                }
                //
            },
            cell);
    } // namespace lspadding

    [[nodiscard]] std::tuple<std::array<int, 4>, std::array<Vertex, 4>> normalize(const Tet &tet) const {
        std::array<int, 4> sort_idx = {0, 1, 2, 3};
        std::array<int, 4> scalar   = {scalar_at(tet.vertices[0]), scalar_at(tet.vertices[1]), scalar_at(tet.vertices[2]), scalar_at(tet.vertices[3])};

        std::ranges::sort(sort_idx, {}, [&](auto &&i) {
            return scalar[i];
        });

        std::array<int, 4> result{};
        int k     = 0;
        result[0] = k;
        std::array<Vertex, 4> vertices{
            tet.vertices[sort_idx[0]],
            tet.vertices[sort_idx[1]],
            tet.vertices[sort_idx[2]],
            tet.vertices[sort_idx[3]]};

        for (int i = 1; i < 4; ++i) {

            if (scalar[sort_idx[i]] != scalar[sort_idx[i - 1]]) {
                ++k;
            }
            result[i] = k;
        }
        return {result, vertices};
    }

    [[nodiscard]] std::tuple<int, int> max_min_scalar(auto &&cell) const {

        int max = std::numeric_limits<int>::min(), min = std::numeric_limits<int>::max();
        for (auto &&v : cell.vertices) {
            int scalar = scalar_at(v);

            max = std::max(max, scalar);
            min = std::min(min, scalar);
        }
        return {max, min};
    }
    [[nodiscard]] bool
    terminate_cell(auto &&cell) const {
        auto [max, min] = max_min_scalar(cell);
        return max - min <= 1;
    }

    [[nodiscard]] int scalar_at(const Vertex &vertex) const {
        auto value = vertex.dot(scalar_field);
        if (value.denominator() != 1)
            throw std::runtime_error("unexpected non integer vertex, this is a bug!");
        return value.numerator();
    }

    template <typename From, typename To>
    constexpr static inline auto deduce_cell = [](const From &cell) -> CellDone {
        return To::make(cell.vertices);
    };

    [[nodiscard]] CellDone deduce(const Cell &cell) const {
        using namespace std::string_literals;

        return std::visit(
            Overload{
                deduce_cell<Tet13, Tet>,
                deduce_cell<Tet22, Tet>,
                deduce_cell<Tet31, Tet>,
                deduce_cell<Tet112, Tet>,
                deduce_cell<Tet121, Tet>,
                deduce_cell<Tet211, Tet>,
                deduce_cell<Tet1111, Tet>,
                deduce_cell<Wedge33, Wedge>,
                deduce_cell<Wedge1212, Wedge>,
                deduce_cell<Wedge2121, Wedge>,
                deduce_cell<Pyramid311, Pyramid>,
                deduce_cell<Pyramid32, Pyramid>,
                deduce_cell<Pyramid23, Pyramid>,
                [](auto &&cell) -> CellDone {
                    return cell;
                }
                //
            },
            cell);
    }

    [[nodiscard]] Cell classify_tet(const Tet &tet) const {

        auto [normalized, vertices] = normalize(tet);
        if (normalized == std::array{0, 1, 1, 1}) {
            return Tet13::make(std::move(vertices));
        } else if (normalized == std::array{0, 0, 1, 1}) {
            return Tet22::make(std::move(vertices));
        } else if (normalized == std::array{0, 0, 0, 1}) {
            return Tet31::make(std::move(vertices));
        } else if (normalized == std::array{0, 1, 2, 2}) {
            return Tet112::make(std::move(vertices));
        } else if (normalized == std::array{0, 1, 1, 2}) {
            return Tet121::make(std::move(vertices));
        } else if (normalized == std::array{0, 0, 1, 2}) {
            return Tet211::make(std::move(vertices));
        } else if (normalized == std::array{0, 1, 2, 3}) {
            return Tet1111::make(std::move(vertices));
        } else {
            return tet;
        }
    }
    void push_by_type(const Cell &cell, std::vector<CellDone> &result, std::queue<CellToCut> &queue) {
        std::visit(
            [&](auto &&cell) {
                using T = std::decay_t<decltype(cell)>;

                if (terminate_cell(cell)) {
                    result.push_back(deduce(cell));
                } else if constexpr (ContainedIn<T, CellToCut>) {
                    queue.push(CellToCut{cell});
                } else {
                    throw std::runtime_error("unreachable");
                }
            },
            cell);
    }

    std::vector<CellDone>
    cut(const std::vector<Tet> &inputs) {
        std::vector<std::vector<CellDone>> results(omp_get_max_threads());
#pragma omp parallel
        {
            int tid = omp_get_thread_num();

            std::queue<CellToCut> queue;
#pragma omp for nowait
            for (const auto &cell : inputs) {
                push_by_type(classify_tet(cell), results[tid], queue);
            }
            while (!queue.empty()) {
                const auto cell = queue.front();
                queue.pop();
                const auto [a, b] = cut_one_cell(cell);
                push_by_type(a, results[tid], queue);
                push_by_type(b, results[tid], queue);
            }
            assert(queue.empty());
        }
        std::vector<CellDone> result;
        int size = 0;
        for (auto &&r : results) {
            size += r.size();
        }
        result.reserve(size);
        for (auto &&r : results) {
            std::move(r.begin(), r.end(), std::back_inserter(result));
        }

        return result;
    }

    Eigen::VectorX<rint> scalar_field;
};

} // namespace lspadding