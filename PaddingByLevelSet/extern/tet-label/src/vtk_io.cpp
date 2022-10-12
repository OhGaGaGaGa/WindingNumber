#include "vtk_io.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace io {
    template<typename OS, typename Iterator, typename INT>
    void cell_data(OS &os, Iterator first, INT size, const char *value_name,
                   const char *table_name = "my_table") {
        os << "CELL_DATA " << size << "\n";
        os << "SCALARS " << value_name << " float\nLOOKUP_TABLE " << table_name
           << "\n";
        for (int i = 0; i < size; ++i, ++first)
            os << *first << "\n";
    }

    template<typename OS, typename Iterator, typename INT>
    void point_data(OS &os, Iterator first, INT size, const char *value_name,
                    const char *table_name = "my_table") {
        os << "POINT_DATA " << size << "\n";
        os << "SCALARS " << value_name << " float\nLOOKUP_TABLE " << table_name
           << "\n";
        for (int i = 0; i < size; ++i, ++first)
            os << *first << "\n";
    }

    int read_tri_mesh(const std::string &file_name, Eigen::Matrix3Xd &V,
                      Eigen::Matrix3Xi &F, Eigen::VectorXi &label) {
        std::ifstream input_file(file_name);
        if (!input_file.is_open()) {
            std::cout << "Unable to open file";
            return 0;
        }
        std::string line;
        std::string str;
        int N(0), M(0);
        while (getline(input_file, line)) {
            std::istringstream iss(line);
            if (line.find("POINTS") != std::string::npos) {
                iss >> str >> N >> str;
                V.resize(3, N);
                break;
            }
        }
        for (int i = 0; i < N; ++i) {
            std::getline(input_file, line);
            std::istringstream iss(line);
            iss >> V(0, i) >> V(1, i) >> V(2, i);
        }
        while (std::getline(input_file, line)) {
            std::istringstream iss(line);
            if (line.find("CELLS") != std::string::npos) {
                iss >> str >> M >> str;
                F.resize(3, M);
                break;
            }
        }
        for (int i = 0; i < M; ++i) {
            std::getline(input_file, line);
            std::istringstream iss(line);
            iss >> str >> F(0, i) >> F(1, i) >> F(2, i);
        }
        for (int i = 0; i <= M; ++i) {
            std::getline(input_file, line);
        }
        while (getline(input_file, line)) {
            std::istringstream iss(line);
            if (line.find("CELL_DATA") != std::string::npos) {
                iss >> str >> M;
                if (M == F.cols())
                    label.resize(M);
                break;
            }
        }
        while (getline(input_file, line)) {
            std::istringstream iss(line);
            if (line.find("LOOKUP_TABLE") != std::string::npos) {
                break;
            }
        }
        for (int i = 0; i < M; ++i) {
            std::getline(input_file, line);
            std::istringstream iss(line);
            iss >> label[i];
        }
        input_file.close();
        return 1;
    }

    int write_tri_mesh(const std::string &file_name, const Eigen::Matrix3Xd &V,
                       const Eigen::Matrix3Xi &F, const Eigen::VectorXi &flabel) {
        std::ofstream os(file_name);
        if (!os) {
            std::cerr << "save vtk error" << std::endl;
            return 0;
        }
        os << "# vtk DataFile Version 2.0\nTRI\nASCII\nDATASET UNSTRUCTURED_GRID\n";

        os << "POINTS " << V.cols() << " double\n";
        for (int i = 0; i < V.cols(); ++i)
            os << V(0, i) << " " << V(1, i) << " " << V(2, i) << "\n";

        os << "CELLS " << F.cols() << " " << F.cols() * 4 << "\n";
        for (int i = 0; i < F.cols(); ++i)
            os << 3 << "  " << F(0, i) << " " << F(1, i) << " " << F(2, i) << "\n";
        os << "CELL_TYPES " << F.cols() << "\n";
        for (int i = 0; i < F.cols(); ++i)
            os << 5 << "\n";
        if (flabel.size() > 0) {
            if (flabel.size() == F.cols()) {
                cell_data(os, &flabel[0], flabel.size(), "face_label");
            } else if (flabel.size() == V.cols()) {
                point_data(os, &flabel[0], flabel.size(), "vertex_label");
            }
        }
        os.close();
        return 1;
    }

    int write_tri_mesh_to_patches(const std::string &file_name,
                                  const Eigen::Matrix3Xd &V,
                                  const Eigen::Matrix3Xi &F,
                                  const Eigen::VectorXi &label) {
        if (label.size() != F.cols())
            return 0;
        std::unordered_map<int, std::vector<int>> faces;
        for (int i = 0; i < label.size(); ++i) {
            faces[label[i]].push_back(i);
        }
        for (const auto &f: faces) {
            Eigen::Matrix3Xi fs(3, f.second.size());
            size_t i(0);
            for (auto j: f.second) {
                fs.col(i++) = F.col(j);
            }
            std::stringstream ss;
            ss << file_name << "_" << f.first << ".vtk";
            Eigen::VectorXi L;
            write_tri_mesh(ss.str(), V, fs, L);
        }
        return 1;
    }

    int write_prism(const std::string &filename, const Eigen::Matrix3Xd &V,
                    const Eigen::MatrixXi &C) {
        std::ofstream os(filename);
        if (!os) {
            std::cout << "read file error!" << std::endl;
            return 0;
        }
        os << "# vtk DataFile Version 3.0\nVolume mesh\nASCII\nDATASET "
              "UNSTRUCTURED_GRID\n";
        os << "POINTS " << V.cols() << " double\n";
        for (int i = 0; i < V.cols(); ++i) {
            os << V(0, i) << " " << V(1, i) << " " << V(2, i) << "\n";
        }
        os << "CELLS " << C.cols() << " " << C.cols() * 7 << "\n";
        for (int i = 0; i < C.cols(); ++i) {
            os << 6 << " " << C(3, i) << " " << C(4, i) << " " << C(5, i) << " "
               << C(0, i) << " " << C(1, i) << " " << C(2, i) << "\n";
        }
        os << "CELL_TYPES " << C.cols() << "\n";
        for (int i = 0; i < C.cols(); ++i) {
            os << 13 << "\n";
        }
        os.close();
        return 1;
    }

    int write_vectors(const std::string &filename, const Eigen::Matrix3Xd &V,
                      const Eigen::Matrix2Xi &E) {
        std::ofstream os(filename);
        if (!os) {
            std::cout << "read file error!" << std::endl;
            return 0;
        }
        os << "# vtk DataFile Version 2.0\nTRI\nASCII\n\nDATASET "
              "UNSTRUCTURED_GRID\n";

        os << "POINTS " << V.cols() << " float\n";
        for (int i = 0; i < V.cols(); ++i)
            os << V(0, i) << " " << V(1, i) << " " << V(2, i) << "\n";

        os << "CELLS " << E.cols() << " " << E.cols() * 3 << "\n";
        for (int i = 0; i < E.cols(); ++i)
            os << 2 << " " << E(0, i) << " " << E(1, i) << "\n";

        os << "CELL_TYPES " << E.cols() << "\n";
        for (int i = 0; i < E.cols(); ++i)
            os << 3 << "\n";

        return 1;
    }

} // namespace io