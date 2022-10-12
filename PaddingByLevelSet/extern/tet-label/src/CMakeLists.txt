cmake_minimum_required(VERSION 3.14)

# find_package(spdlog REQUIRED)
# find_package(Eigen3 3.3 REQUIRED)
# find_package(tetgen REQUIRED)
# find_package(OpenMesh REQUIRED)
# find_package(OpenVolumeMesh REQUIRED)

# tet_labeling
add_executable(tet_labeling tet_labeling.cpp vtk_io.cpp)
target_include_directories(tet_labeling PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${tetgen_SOURCE_DIR}
        ${OpenVolumeMesh_SOURCE_DIR}
        ${Eigen_SOURCE_DIR})

target_link_libraries(tet_labeling PRIVATE
        spdlog::spdlog
        Eigen3::Eigen
        tetgen
        OpenMeshCore OpenMeshTools
        OpenVolumeMesh::OpenVolumeMesh
        VTK::IOLegacy
        CLI11::CLI11)

# extract_lines
add_executable(extract_lines extract_lines.cpp)
target_include_directories(extract_lines PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(extract_lines PRIVATE
        spdlog::spdlog
        Eigen3::Eigen
        OpenVolumeMesh::OpenVolumeMesh
        VTK::IOLegacy
        )

# label_restore
add_executable(label_restore label_restore.cpp label_restore.cpp)
#target_link_directories(label_restore PRIVATE ${vtk_SOURCE_DIR})
target_link_libraries(label_restore PRIVATE
        spdlog::spdlog
        Eigen3::Eigen
        VTK::IOLegacy)