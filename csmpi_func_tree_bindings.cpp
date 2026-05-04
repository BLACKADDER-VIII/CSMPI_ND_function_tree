#include "func_tree.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace py = pybind11;

static py::dict get_func_tree(
    std::string graph_path,
    std::vector<std::unordered_map<std::string, std::string>> sym_tab_maps,
    int num_proc,
    std::vector<igraph_integer_t> nd_nodes)
{
    FuncTree ft(graph_path, sym_tab_maps, num_proc, nd_nodes);
    // Extract all data into plain vectors before ft (and its igraph objects) destructs
    std::vector<std::pair<int,int>> edges      = ft.get_edge_list();
    std::vector<std::string>        func_names = ft.get_func_names();
    std::vector<double>             nd_scores  = ft.get_nd_scores();
    py::dict result;
    result["edges"]      = edges;
    result["func_names"] = func_names;
    result["nd_scores"]  = nd_scores;
    return result;
    // ft destructs here, releasing evg and func_g
}

PYBIND11_MODULE(csmpi_func_tree, m) {
    m.doc() = "Pybind11 bridge for the CSMPI function-tree backend";
    m.def("get_func_tree", &get_func_tree,
          py::arg("graph_path"),
          py::arg("sym_tab_maps"),
          py::arg("num_proc"),
          py::arg("nd_nodes"),
          R"(Build the annotated function call-graph.

Returns a dict with:
  edges      – list of (src, dst) int pairs (edge list)
  func_names – list of str, one per vertex (func_name attribute)
  nd_scores  – list of float, one per vertex (ND score)
)");
}
