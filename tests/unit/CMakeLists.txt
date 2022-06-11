enable_testing()

add_executable(
    test_simplebyteray
    test_simplebyteray.cxx
)
target_link_libraries(
    test_simplebyteray
    byteray
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_simplebyteray)
