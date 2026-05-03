from py_api_connector import get_func_tree
import igraph as ig

nd_nodes = list(range(1, 100))

evg_path = "/home/exouser/amr_dumpi_files/event_graph.graphml"
sym_tab_dir = "/home/exouser/amr_dumpi_files"

fg = get_func_tree(evg_path, nd_nodes, sym_tab_dir, 8)

fg.write_graphml("TestFuncGraph.graphml")