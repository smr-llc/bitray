
add_library(bitray
    byteray.cc
    filechunkcache.cc
)

target_include_directories(bitray
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)