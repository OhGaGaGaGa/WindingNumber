#pragma once

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>


#include <Eigen/Dense>

#include "mesh.hpp"

namespace io {
    int read_tri_mesh(const std::string &file_name, Eigen::Matrix3Xd &V,
                      Eigen::Matrix3Xi &F, Eigen::VectorXi &label);

    int write_tri_mesh(const std::string &file_name, const Eigen::Matrix3Xd &V,
                       const Eigen::Matrix3Xi &F, const Eigen::VectorXi &flabel);

    int write_tri_mesh_to_patches(const std::string &file_name,
                                  const Eigen::Matrix3Xd &V,
                                  const Eigen::Matrix3Xi &F,
                                  const Eigen::VectorXi &label);

    int write_prism(const std::string &filename, const Eigen::Matrix3Xd &V,
                    const Eigen::MatrixXi &C);

    int write_vectors(const std::string &filename, const Eigen::Matrix3Xd &V,
                      const Eigen::Matrix2Xi &E);

}; // namespace io

namespace mesh_tools {
    namespace io {

        inline int read_vtk(std::vector<double> &points,
                            std::vector<std::size_t> &cells,
                            std::vector<std::size_t> &cell_types,
                            const std::string &file_path);

        inline int write_vtk(const std::vector<double> &points,
                             const std::vector<std::size_t> &cells,
                             const std::vector<std::size_t> &cell_types,
                             const std::string &file_path,
                             const std::string header);

        template<typename Point>
        inline mesh::Mesh<Point> ReadVtk(const std::string &path);

        template<typename Point>
        int WriteVtk(const mesh::Mesh<Point> &mesh, const std::string &path,
                     const std::string &header);

// ===== Implement =====
        inline int read_points(std::vector<double> &points, std::ifstream &fin) {
            points.clear();
            size_t n;
            fin >> n;
            std::string data_type;
            fin >> data_type;
            for (size_t i = 0; i < n; ++i) {
                double x, y, z;
                fin >> x >> y >> z;
                points.push_back(x);
                points.push_back(y);
                points.push_back(z);
            }
            return 0;
        };

        inline int read_cells(std::vector<size_t> &cells, std::ifstream &fin) {
            cells.clear();
            size_t n, m, check_m = 0;
            fin >> n >> m;
            for (size_t i = 0; i < n; ++i) {
                size_t c;
                fin >> c;
                cells.push_back(c);
                for (size_t j = 0; j < c; ++j) {
                    size_t x;
                    fin >> x;
                    cells.push_back(x);
                }
                check_m += c + 1;
            }
            if (m != check_m) {
                std::cerr << "ERROR: m: " << m << " check_m: " << check_m << std::endl;
                exit(-5);
            }
            return 0;
        }

// Temporary
        inline int read_cell_data(std::vector<int> &cell_data, std::ifstream &fin) {
            cell_data.clear();
            int numCells;
            fin >> numCells;
            std::string s;
            fin >> s >> s >> s >> s >> s;
            for (int i = 0; i < numCells; ++i) {
                int x;
                fin >> x;
                cell_data.push_back(x);
            }
            return 0;
        }

        inline int read_cell_types(std::vector<size_t> &cell_types,
                                   std::ifstream &fin) {
            cell_types.clear();
            size_t n;
            fin >> n;
            for (size_t i = 0; i < n; ++i) {
                size_t x;
                fin >> x;
                cell_types.push_back(x);
            }
            return 0;
        }

        inline int read_vtk(std::vector<double> &points,
                            std::vector<std::size_t> &cells,
                            std::vector<std::size_t> &cell_types,
                            const std::string &file_path) {
            std::ifstream fin(file_path, std::ios::in);
            if (!fin.is_open()) {
                std::cerr << "ERROR: Fail to open file " << file_path << std::endl;
                return -1;
            }

            char version[100];
            fin.getline(version, 100);

            char header[1024];
            fin.getline(header, 1024);

            std::string file_type;
            fin >> file_type;
            transform(file_type.begin(), file_type.end(), file_type.begin(), ::tolower);
            if (file_type != "ascii") {
                std::cerr << "WARNING: File type should be ascii but it is "
                          << file_type << std::endl;
                return -2;
            }

            std::string dataset, dataset_type;
            fin >> dataset >> dataset_type;
            transform(dataset.begin(), dataset.end(), dataset.begin(), ::tolower);
            transform(dataset_type.begin(), dataset_type.end(), dataset_type.begin(),
                      ::tolower);
            if (dataset != "dataset") {
                std::cerr << "WARNING: Done find the dataset keyword." << std::endl;
                return -3;
            }
            if (dataset_type != "unstructured_grid") {
                std::cerr
                        << "WARNING: Dataset type should be unstructured_grid but it is "
                        << dataset_type << std::endl;
                return -4;
            }

            bool point_flag = false, cell_flag = false, cell_type_flag = false;
            std::string type;
            for (size_t i = 0; i < 3; ++i) {
                fin >> type;
                transform(type.begin(), type.end(), type.begin(), ::tolower);
                if (type == "points" && !point_flag) {
                    read_points(points, fin);
                    point_flag = true;
                } else if (type == "cells" && !cell_flag) {
                    read_cells(cells, fin);
                    cell_flag = true;
                } else if (type == "cell_types" && !cell_type_flag) {
                    read_cell_types(cell_types, fin);
                    cell_type_flag = true;
                } else {
                    std::cerr << "WARNING: There is a unknow keyword " + type
                              << std::endl;
                }
            }

            fin.close();

            return 0;
        }

// Temporary
        inline int read_vtk_with_cell_data(std::vector<double> &points,
                                           std::vector<std::size_t> &cells,
                                           std::vector<std::size_t> &cell_types,
                                           std::vector<int> &cell_data,
                                           const std::string &file_path) {
            std::ifstream fin(file_path, std::ios::in);
            if (!fin.is_open()) {
                std::cerr << "ERROR: Fail to open file " << file_path << std::endl;
                return -1;
            }

            char version[100];
            fin.getline(version, 100);

            char header[1024];
            fin.getline(header, 1024);

            std::string file_type;
            fin >> file_type;
            transform(file_type.begin(), file_type.end(), file_type.begin(), ::tolower);
            if (file_type != "ascii") {
                std::cerr << "WARNING: File type should be ascii but it is "
                          << file_type << std::endl;
                return -2;
            }

            std::string dataset, dataset_type;
            fin >> dataset >> dataset_type;
            transform(dataset.begin(), dataset.end(), dataset.begin(), ::tolower);
            transform(dataset_type.begin(), dataset_type.end(), dataset_type.begin(),
                      ::tolower);
            if (dataset != "dataset") {
                std::cerr << "WARNING: Done find the dataset keyword." << std::endl;
                return -3;
            }
            if (dataset_type != "unstructured_grid") {
                std::cerr
                        << "WARNING: Dataset type should be unstructured_grid but it is "
                        << dataset_type << std::endl;
                return -4;
            }

            bool point_flag = false, cell_flag = false, cell_type_flag = false,
                    cell_data_flag = false;
            std::string type;
            for (size_t i = 0; i < 4; ++i) {
                fin >> type;
                transform(type.begin(), type.end(), type.begin(), ::tolower);
                if (type == "points" && !point_flag) {
                    read_points(points, fin);
                    point_flag = true;
                } else if (type == "cells" && !cell_flag) {
                    read_cells(cells, fin);
                    cell_flag = true;
                } else if (type == "cell_types" && !cell_type_flag) {
                    read_cell_types(cell_types, fin);
                    cell_type_flag = true;
                } else if (type == "cell_data" && !cell_data_flag) {
                    read_cell_data(cell_data, fin);
                    cell_data_flag = true;
                } else {
                    std::cerr << "WARNING: There is a unknow keyword " + type
                              << std::endl;
                }
            }

            fin.close();

            return 0;
        }

        inline int write_vtk(const std::vector<double> &points,
                             const std::vector<std::size_t> &cells,
                             const std::vector<std::size_t> &cell_types,
                             const std::string &file_path,
                             const std::vector<int> &labels = std::vector<int>{},
                             const std::string header = "May the world peace forever") {
            std::ofstream fout(file_path, std::ios::out);
            if (!fout.is_open()) {
                std::cerr << "ERROR: Fail to open file " << file_path << std::endl;
                return -6;
            }

            fout << "# vtk DataFile Version 2.0" << std::endl;
            fout << header << std::endl;
            fout << "ASCII" << std::endl;
            fout << "DATASET UNSTRUCTURED_GRID" << std::endl;
            fout << "POINTS " << points.size() / 3 << " double" << std::endl;
            for (size_t pid = 0; pid < points.size() / 3; ++pid) {
                fout << std::setprecision(18) << points[3 * pid + 0] << " ";
                fout << std::setprecision(18) << points[3 * pid + 1] << " ";
                fout << std::setprecision(18) << points[3 * pid + 2] << std::endl;
            }

            fout << "CELLS " << cell_types.size() << " " << cells.size() << std::endl;
            for (size_t cid = 0, p = 0; cid < cell_types.size(); ++cid) {
                size_t c = cells[p++];
                fout << c;
                for (size_t i = 0; i < c; ++i)
                    fout << " " << cells[p++];
                fout << std::endl;
            }

            fout << "CELL_TYPES " << cell_types.size() << std::endl;
            for (size_t x: cell_types)
                fout << x << std::endl;

            if (!labels.empty()) {
                // assert(labels.size() == points.size() / 3);
                fout << "POINT_DATA " << points.size() / 3 << "\n";
                fout << "SCALARS " << "padding" << " int\nLOOKUP_TABLE " << "my_table"
                     << "\n";
                for (int label: labels)
                    fout << label << "\n";

               // write zeros to the rest of the points
               for (int i = 0; i < points.size() / 3 - labels.size(); ++i)
                   fout << 0 << "\n";
            }
            fout.close();
            return 0;
        }

        inline int write_vtk_region(const std::vector<int> &region_id,
                                    const std::string &file_path) {
            std::ofstream fout(file_path, std::ios::app);
            if (!fout.is_open()) {
                std::cerr << "ERROR: Fail to open file " << file_path << std::endl;
                return -6;
            }

            fout << "CELL_DATA " << region_id.size() << std::endl;
            fout << "SCALARS RegionId vtkIdType" << std::endl;
            fout << "LOOKUP_TABLE default" << std::endl;
            for (int id: region_id) {
                fout << id << std::endl;
            }

            fout.close();

            return 0;
        }

        template<typename Point>
        inline mesh::Mesh<Point> ReadVtk(const std::string &path) {
            return mesh::Mesh<Point>();
        }

        template<typename Point>
        int WriteVtk(const mesh::Mesh<Point> &mesh, const std::string &path,
                //flabel
                     const std::vector<int> &flabel = std::vector<int>(),
                     const std::string header = "May the world peace forever") {
            std::vector<double> points;
            std::vector<std::size_t> cells;
            std::vector<std::size_t> cell_types;
            for (size_t i = 0; i < mesh.NumVertices(); ++i) {
                Point p = mesh.GetPoint(i);
                points.push_back(p[0]);
                points.push_back(p[1]);
                points.push_back(p[2]);
            }
            for (size_t i = 0; i < mesh.NumCells(); ++i) {
                mesh_tools::mesh::Cell cell = mesh.GetCell(i);
                cells.push_back(cell.vertices.size());
                cells.insert(cells.end(), cell.vertices.begin(), cell.vertices.end());
                cell_types.push_back(cell.type);
            }
            write_vtk(points, cells, cell_types, path, flabel, header);
            return 0;

        }
    } // namespace io
} // namespace mesh_tools