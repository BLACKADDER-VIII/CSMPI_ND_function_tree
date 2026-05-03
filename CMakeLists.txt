cmake_minimum_required(VERSION 3.15)
project(csmpi_func_tree CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)
find_package(igraph REQUIRED)
find_package(OpenMP REQUIRED)

pybind11_add_module(csmpi_func_tree
    csmpi_func_tree_bindings.cpp
    func_tree.cpp
)

target_link_libraries(csmpi_func_tree PRIVATE
    igraph::igraph
    OpenMP::OpenMP_CXX
)

# Place the .so next to py_api_connector.py so it can be imported directly
set_target_properties(csmpi_func_tree PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
