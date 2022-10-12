enable_testing()
CPMAddPackage(
    NAME "doctest"
    GIT_REPOSITORY "https://__mirror:mirror@ryon.ren:2443/mirrors/doctest.git"
    GIT_TAG "v2.4.9")
add_executable(lspadding_test
    ./test.cpp ./test_cut.cpp ./test_extract.cpp
    ./test_common.cpp ./test_read_vtk.cpp ./test_integrated.cpp)
add_test(NAME lspadding_test COMMAND lspadding_test)
target_link_libraries(lspadding_test PRIVATE
    doctest::doctest lspadding_lib)