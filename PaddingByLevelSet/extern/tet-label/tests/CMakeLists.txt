enable_testing()
project(tests)

CPMAddPackage(
    NAME "doctest"
    GIT_REPOSITORY "https://github.com/doctest/doctest.git"
    GIT_TAG "v2.4.9")

find_package(OpenVolumeMesh REQUIRED)

add_executable(${PROJECT_NAME} ./test.cpp ./test_internal_vertex_label.cpp)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${doctest_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenVolumeMesh_SOURCE_DIR})
message("${doctest_SOURCE_DIR}")
message("${OpenVolumeMesh_SOURCE_DIR}")
message("${OpenVolumeMesh}")
message("${OpenVolumeMeshCore}")
message("${OpenVolumeMesh_LIBS}")

target_link_libraries(${PROJECT_NAME} PRIVATE doctest::doctest)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenVolumeMesh::OpenVolumeMesh)
