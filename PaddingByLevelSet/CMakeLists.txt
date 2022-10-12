cmake_minimum_required(VERSION 3.23)
project(LSPadding)

include(cmake/get_cpm.cmake)

include(./dependencies.cmake)
add_compile_options(-fdiagnostics-color)

find_package(OpenMP REQUIRED)

add_library(lspadding_lib INTERFACE)
target_link_libraries(lspadding_lib INTERFACE
    Eigen3::Eigen VTK::IOLegacy spdlog::spdlog fmt::fmt nameof::nameof OpenMP::OpenMP_CXX)
target_include_directories(lspadding_lib INTERFACE include)
target_compile_features(lspadding_lib INTERFACE cxx_std_20)

add_executable(lspadding src/main.cpp)
target_link_libraries(lspadding PUBLIC lspadding_lib)
target_compile_features(lspadding PUBLIC cxx_std_20)

add_executable(lspadding_pure src/notremove.cpp)
target_link_libraries(lspadding_pure PUBLIC lspadding_lib)
target_compile_features(lspadding_pure PUBLIC cxx_std_20)

enable_testing()
add_subdirectory(tests)

add_subdirectory(extern)

install(TARGETS 
        lspadding_pure tet_labeling extract_lines label_restore 
        spdlog fmt nameof IOLegacy
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
