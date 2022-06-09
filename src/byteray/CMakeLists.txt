
add_library(byteray
    byteray.cxx
    filechunkcache.cxx
)

target_include_directories(byteray
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)