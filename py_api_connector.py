import igraph as ig



def get_symbol_table_dict(sym_table_path: str) -> dict:
    sym_dict = {}
    with open(sym_table_path, 'r') as f:
        while f:
            line = f.readline().split(' ')
            sym_dict[line[0]] = line[1]
    return sym_dict




def make_func_tree(graph_path: str, nd_nodes: list[int], sym_tab_paths: list[str], num_proc: int) -> ig.Graph:
    assert len(sym_tab_paths) == num_proc, "#Ranks should match the #sym_tab_paths provided"
    sym_tabs = []
    # Making symbol tables for each rank
    for i in range(num_proc):
        sym_tabs.append(get_symbol_table_dict(sym_tab_paths[i]))
    
    
