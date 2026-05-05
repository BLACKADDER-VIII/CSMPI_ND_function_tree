import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

import igraph as ig
import csmpi_func_tree


def get_symbol_table_dict(sym_table_path: str) -> dict:
    sym_dict = {}
    with open(sym_table_path, 'r') as f:
        for line in f:
            addr, sep, name = line.rstrip('\n').partition('\t')
            if sep and addr and name:
                sym_dict[addr] = name
    return sym_dict


def _make_func_tree(graph_path: str, nd_nodes: list[int], sym_tab_paths: list[str], num_proc: int) -> ig.Graph:
    assert len(sym_tab_paths) == num_proc, "#Ranks should match the #sym_tab_paths provided"

    sym_tabs = []
    for i in range(num_proc):
        sym_tabs.append(get_symbol_table_dict(sym_tab_paths[i]))
    result = csmpi_func_tree.get_func_tree(
        graph_path=graph_path,
        sym_tab_maps=sym_tabs,
        num_proc=num_proc,
        nd_nodes=nd_nodes,
    )

    g = ig.Graph(edges=result["edges"], directed=True)
    g.vs["func_name"] = result["func_names"]
    g.vs["nd_score"]  = result["nd_scores"]
    return g


def get_func_tree(graph_path: str, nd_nodes: list[int], sym_tabs_dir_path: str, num_proc: int) -> ig.Graph:
    sym_tab_paths = [f"{sym_tabs_dir_path}/rank_{i}.symtab" for i in range(num_proc)]

    return _make_func_tree(graph_path, nd_nodes, sym_tab_paths, num_proc)

