
add_library(byteray
    byteray.cc
    filechunkcache.cc
)

target_include_directories(byteray
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)