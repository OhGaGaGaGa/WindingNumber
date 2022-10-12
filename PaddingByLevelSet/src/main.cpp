#include "lspadding/celltypes.hpp"
#include "lspadding/cutting.hpp"
#include "lspadding/read_vtk.hpp"
#include "lspadding/extract_mesh.hpp"

#include <fstream>
#include <limits>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
        return 1;
    }
    std::string input_file  = argv[1];
    std::string output_file = argv[2];

    auto [scalars, vertices, tets] = lspadding::read_vtk(input_file);

    int M = std::numeric_limits<int>::min();

    for (int i = 0; i < scalars.size(); i++) {
        if (scalars(i) > 0)
            scalars(i) += 2;
        M = std::max(M, scalars(i));
    }
    lspadding::CutContext context(scalars);
    auto grid = context.cut(tets);

    std::vector<lspadding::CellDone> filterd_grid;
    for (auto &&cell : grid) {
        bool non_surface = std::visit(
            [&](auto &&cell) {
                auto [max, min] = context.max_min_scalar(cell);
                return !(max == M && min == M - 1);
            },
            cell);
        if (non_surface) {
            filterd_grid.push_back(std::move(cell));
        }
    }

    lspadding::extract::ExtractContext extract_context(scalars, vertices);
    auto [extracted_cells, vertices_vector] = extract_context.extract<true>(filterd_grid);
    auto output                             = extract_context.output<true>(extracted_cells);
    std::ofstream fout(output_file);
    fout << output;
    return 0;
}