#include <iostream>

#include <igl/marching_tets.h>
#include <igl/writeOBJ.h>
#include "IO.h"
#include "WindingNumber.h"

using namespace std;

int main(int argc, char const* argv[]) {
    if (argc != 1 && argc != 1 + 7) {
        cout << "Invalid Arguments Count. " << '\n';
        return 0;
    }
    vector<string> filepath;
    for (int i = 1; i < argc; i++) 
        filepath.emplace_back(argv[i]);
    IO ioprocesser(argc, filepath);
    
    Eigen::MatrixXd inputV;
    Eigen::MatrixXi inputF;
    Eigen::MatrixXi inputTet;

    bool input_file_status = ioprocesser.input_triangle_mesh(inputV, inputF);
    if (!input_file_status) {
        cout << "Input File Error. \n";
        return 0;
    }
    auto [gridV, gridCube] = generate_grid::generate_grid(inputV);
    auto [w, judge] = winding_number::process_winding_number(gridV /* generate_ramdom_points::get_random_points(inputV) */, Meshs(inputV, inputF));

    bool output_file_status = ioprocesser.output_files(
        inputV,  inputF, inputTet, 
        gridV /* generate_ramdom_points::get_random_points(inputV) */, w, judge, 
        gridV, gridCube
    );
    if (!output_file_status) 
        cout << "Output Error. " << '\n';
    
    return 0;
}