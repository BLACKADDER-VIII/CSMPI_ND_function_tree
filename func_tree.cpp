#include "func_tree.hpp"
#include "callstack_parse_utilities.cpp"


FuncTree::set_callstack(){
    for (igraph_integer_t i = 0; i<igraph_strvector_size(&(this->attr_callstack)); i++){
        this->callstack[i] = get_callstack_vec(igraph_strvector_at(&(this->attr_callstack), i));
    }
}

FuncTree::create_func_graph(){
    std::set<std::string> func_nodes;
    std::vector<std::string> func_node_vec;
    std::unordered_map<std::string, int> func_name_to_id_map;
    std::vector<int> func_edges;
    // Adding nodes
    for (igraph_integer_t i = 0; i<this->callstack.size(); i++){
        int p = VECTOR(pid)[i];
        std::unordered_map<std::string, std::string> sym_tab_dict = this->sym_tab_maps[p];
        std::vector<std::string> cs = this->callstack[i];
        for (int j = 0; j<cs.size(); j++){
            func_nodes.add(sym_tab_dict[cs[j]]);
        }
    }
    func_node_vec.assign(func_nodes.begin(), func_nodes.end());
    // Making node ID map
    for (int i = 0; i<func_node_vec.size(); i++){
        func_name_to_id_map[func_node_vec[i]] = i;
    }
    // Making edges
    for (igraph_integer_t i = 0; i<this->callstack.size(); i++){
        int p = VECTOR(pid)[i];
        std::unordered_map<std::string, std::string> sym_tab_dict = this->sym_tab_maps[p];
        std::vector<std::string> cs = this->callstack[i];
        for (int j = 0; j<cs.size()-1; j++){
            int u = func_name_to_id_map[sym_tab_dict[cs[i]]];
            int v = func_name_to_id_map[sym_tab_dict[cs[i+1]]];
            func_edges.push_back(u);
            func_edges.push_back(v);
        }
    }
    this->func_name_to_id_map = func_name_to_id_map;
    igraph_vector_int_t edges;
    igraph_vector_int_init_array(&edges, func_edges.data(), (igraph_integer_t)func_edges.size());
    igraph_strvector_t attr_func_names;
    igraph_strvector_init(&attr_func_names, 0);
    for (std::string fn: func_node_vec){
        igraph_strvector_push_back(&attr_func_names, fn);
    }
    igraph_add_edges(&(this->func_g), &edges);
    SETVASV(&(this->func)g, "func_name", &attr_func_names);

}