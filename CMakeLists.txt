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

add_executable(wn src/main.cpp src/Meshs.cpp)
target_link_libraries(wn PUBLIC wn_lib)
target_compile_features(wn PUBLIC cxx_std_20)