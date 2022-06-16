find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Generate API XML for Sphinx/breathe with Doxygen 
get_target_property(BITRAY_HEADER_DIRS bitray INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE BITRAY_HEADERS ${BITRAY_HEADER_DIRS}/*.h)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src/bitray)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_XML_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(OUTPUT ${DOXYGEN_XML_FILE}
                   DEPENDS ${BITRAY_HEADERS}
                   COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                   MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                   COMMENT "Generating API XML with Doxygen")

add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_XML_FILE})

# Build docs with Sphinx
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

file(GLOB_RECURSE RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.rst)
list(APPEND RST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../README.rst)

add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND 
                     ${SPHINX_EXECUTABLE} -b html
                     -Dbreathe_projects.bitray=${DOXYGEN_OUTPUT_DIR}/xml
                     ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                     ${RST_FILES}
                     ${DOXYGEN_XML_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

add_custom_target(Sphinx ALL DEPENDS ${SPHINX_INDEX_FILE})