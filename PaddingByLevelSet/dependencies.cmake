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