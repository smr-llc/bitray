
add_executable(bm_vector bm_vector.cxx)
target_link_libraries(bm_vector benchmark::benchmark)

add_executable(bm_byteray bm_byteray.cxx)
target_link_libraries(bm_byteray benchmark::benchmark byteray)