#include "lspadding/celltypes.hpp"
#include "lspadding/cutting.hpp"
#include "lspadding/read_vtk.hpp"
#include "lspadding/extract_mesh.hpp"

#include <fstream>
#include <limits>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input> <output_vtk> <output_vertices>" << std::endl;
        return 1;
    }
    std::string input_file           = argv[1];
    std::string output_vtk_file      = argv[2];
    std::string output_vertices_file = argv[3];

    auto [scalars, vertices, tets] = lspadding::read_vtk(input_file);

    lspadding::CutContext context(scalars);
    auto grid = context.cut(tets);
    lspadding::extract::ExtractContext extract_context(scalars, vertices);
    auto [extracted_cells, vertices_vector] = extract_context.extract(grid);
    auto output_vtk                         = extract_context.output<true>(extracted_cells);
    std::ofstream fout_vtk(output_vtk_file);
    fout_vtk << output_vtk;

    auto output_vertex = extract_context.output(vertices_vector);
    std::ofstream fout_vertices(output_vertices_file);
    fout_vertices << output_vertex;
    return 0;
}