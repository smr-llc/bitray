enable_testing()

add_executable(
    test_simplebyteray
    test_simplebyteray.cc
)
target_link_libraries(
    test_simplebyteray
    bitray
    gtest_main
)

add_executable(
    test_bigbyteray
    test_bigbyteray.cc
)
target_link_libraries(
    test_bigbyteray
    bitray
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_simplebyteray test_bigbyteray)
