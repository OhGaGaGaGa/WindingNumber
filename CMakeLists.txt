cmake_minimum_required(VERSION 3.23)
project(WindingNumber)

include(cmake/get_cpm.cmake)

include(./dependencies.cmake)
add_compile_options(-fdiagnostics-color)

find_package(OpenMP REQUIRED)

add_library(wn_lib INTERFACE)
target_link_libraries(wn_lib INTERFACE
    igl::core igl::png Eigen3::Eigen VTK::IOLegacy spdlog::spdlog fmt::fmt nameof::nameof OpenMP::OpenMP_CXX)
target_include_directories(wn_lib INTERFACE include)
target_compile_features(wn_lib INTERFACE cxx_std_20)

add_executable(wn src/main.cpp)
target_link_libraries(wn PUBLIC wn_lib)
target_compile_features(wn PUBLIC cxx_std_20)

# enable_testing()
# add_subdirectory(tests)

# add_subdirectory(extern)

# install(TARGETS 
#         lspadding_pure tet_labeling extract_lines label_restore 
#         spdlog fmt nameof IOLegacy
#         RUNTIME DESTINATION bin
#         LIBRARY DESTINATION lib
#         ARCHIVE DESTINATION lib)
