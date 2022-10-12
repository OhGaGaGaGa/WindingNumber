cmake_minimum_required(VERSION 3.14)

project(tet_labeling)
set(CMAKE_CXX_STANDARD 17)

include(cmake/CPM.cmake)
CPMAddPackage(
        NAME "tetgen"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/tetgen.git"
        GIT_TAG "4f3bfba3997f20aa1f96cfaff604313a8c2c85b6")

CPMAddPackage(
        NAME "OpenMesh"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/OpenMesh.git"
        GIT_TAG "OpenMesh-8.1")

CPMAddPackage(
        NAME "OpenVolumeMesh"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/OpenVolumeMesh.git"
        GIT_TAG "v3.1.0")

CPMAddPackage(
        NAME "Eigen3"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/eigen3.git"
        GIT_TAG "3.4.0"
        OPTIONS
        "BUILD_TESTING OFF"
        "EIGEN_BUILD_DOC OFF"
        "EIGEN_LEAVE_TEST_IN_ALL_TARGET OFF"
        "EIGEN_BUILD_PKGCONFIG OFF")

CPMAddPackage(
        NAME "spdlog"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/spdlog.git"
        GIT_TAG "v1.10.0")

CPMAddPackage(
        NAME "VTK"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/VTK.git"
        GIT_TAG "v9.1.0"
        GIT_SUBMODULES ""
        OPTIONS
        "VTK_ENABLE_LOGGING OFF"
        "VTK_ENABLE_REMOTE_MODULES OFF"
        "VTK_FORBID_DOWNLOADS ON"
        "VTK_GROUP_ENABLE_Imaging DONT_WANT"
        "VTK_GROUP_ENABLE_MPI DONT_WANT"
        "VTK_GROUP_ENABLE_Qt DONT_WANT"
        "VTK_GROUP_ENABLE_Rendering DONT_WANT"
        "VTK_GROUP_ENABLE_StandAlone DONT_WANT"
        "VTK_GROUP_ENABLE_Views DONT_WANT"
        "VTK_GROUP_ENABLE_Web DONT_WANT"
        "VTK_INSTALL_SDK OFF"
        "VTK_ENABLE_WRAPPING OFF"
        "VTK_MODULE_ENABLE_VTK_IOLegacy YES")

CPMAddPackage(
        NAME "CLI"
        GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/CLI11.git"
        GIT_TAG "v2.2.0")

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)