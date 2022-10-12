CPMAddPackage(
    NAME "Eigen3"

    # GIT_REPOSITORY "https://github.com/hexagon-geo-surv/eigen.git"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/eigen3.git"
    GIT_TAG "3.4.0"
    OPTIONS
    "BUILD_TESTING OFF"
    "EIGEN_BUILD_DOC OFF"
    "EIGEN_LEAVE_TEST_IN_ALL_TARGET OFF"
    "EIGEN_BUILD_PKGCONFIG OFF"
)

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
    "VTK_MODULE_ENABLE_VTK_IOLegacy YES"
)

CPMAddPackage(
    NAME "fmt"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/fmt.git"
    GIT_TAG "8.0.1"
)
CPMAddPackage(
    NAME "spdlog"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/spdlog.git"
    GIT_TAG "v1.9.2"
)
CPMAddPackage(
    NAME "nameof"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/nameof.git"
    GIT_TAG "v0.10.1"
)
