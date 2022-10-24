cmake_minimum_required(VERSION 3.23)
project(WindingNumber)

include(cmake/get_cpm.cmake)

include(./dependencies.cmake)
add_compile_options(-fdiagnostics-color)

add_library(wn_lib INTERFACE)
target_link_libraries(wn_lib INTERFACE Eigen3::Eigen igl::core)
target_include_directories(wn_lib INTERFACE include)
target_compile_features(wn_lib INTERFACE cxx_std_20)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(wn src/main.cpp src/random_points.cpp src/Meshs.cpp src/VTKwriter.cpp)
target_link_libraries(wn PUBLIC wn_lib)
target_compile_features(wn PUBLIC cxx_std_20)