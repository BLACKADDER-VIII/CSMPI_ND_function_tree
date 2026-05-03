#include "func_tree.hpp"
#include "callstack_parse_utilities.cpp"


void FuncTree::set_callstack(){
    for (igraph_integer_t i = 0; i<igraph_strvector_size(&(this->attr_callstack)); i++){
        this->callstack[i] = get_callstack_vec(igraph_strvector_get(&(this->attr_callstack), i));
    }
}

void FuncTree::create_func_graph(){
    std::set<std::string> func_nodes;
    std::vector<std::string> func_node_vec;
    std::unordered_map<std::string, int> func_name_to_id_map;
    std::vector<igraph_integer_t> func_edges;
    // Adding nodes
    #pragma omp parallel for
    for (igraph_integer_t i = 0; i<this->callstack.size(); i++){
        int p = VECTOR(pid)[i];
        std::unordered_map<std::string, std::string> sym_tab_dict = this->sym_tab_maps[p];
        std::vector<std::string> cs = this->callstack[i];
        for (int j = 0; j<cs.size(); j++){
            auto it = sym_tab_dict.find(cs[j]);
            if (it == sym_tab_dict.end()) continue;
            #pragma omp critical
            func_nodes.insert(it->second);
        }
    }
    func_node_vec.assign(func_nodes.begin(), func_nodes.end());
    // Making node ID map
    for (int i = 0; i<func_node_vec.size(); i++){
        func_name_to_id_map[func_node_vec[i]] = i;
    }
    std::cout<<"Starting edge making..."<<std::endl;
    // Making edges
    #pragma omp parallel for
    for (igraph_integer_t i = 0; i<this->callstack.size(); i++){
        int p = VECTOR(pid)[i];
        std::unordered_map<std::string, std::string> sym_tab_dict = this->sym_tab_maps[p];
        std::vector<std::string> cs = this->callstack[i];
        for (igraph_integer_t j = 0; j+1 < (igraph_integer_t)cs.size(); j++){
            auto it_j  = sym_tab_dict.find(cs[j]);
            auto it_j1 = sym_tab_dict.find(cs[j+1]);
            if (it_j == sym_tab_dict.end() || it_j1 == sym_tab_dict.end()){ std::cout<<cs[j]<<" "<<cs[j+1]<<" "<<p<<std::endl; continue;}
            auto it_u = func_name_to_id_map.find(it_j->second);
            auto it_v = func_name_to_id_map.find(it_j1->second);
            if (it_u == func_name_to_id_map.end() || it_v == func_name_to_id_map.end()) continue;
            #pragma omp critical
            {
            func_edges.push_back(it_u->second);
            func_edges.push_back(it_v->second);
            }
        }
    }
    this->func_name_to_id_map = func_name_to_id_map;
    igraph_vector_int_t edges;
    igraph_vector_int_init_array(&edges, func_edges.data(), (igraph_integer_t)func_edges.size());
    igraph_strvector_t attr_func_names;
    igraph_strvector_init(&attr_func_names, 0);
    for (std::string fn: func_node_vec){
        igraph_strvector_push_back(&attr_func_names, fn.c_str());
    }
    igraph_empty(&(this->func_g), (igraph_integer_t)func_node_vec.size(), IGRAPH_DIRECTED);
    igraph_add_edges(&(this->func_g), &edges, NULL);
    SETVASV(&(this->func_g), "func_name", &attr_func_names);

}

std::vector<std::pair<int,int>> FuncTree::get_edge_list() const {
    igraph_vector_int_t edges_v;
    igraph_vector_int_init(&edges_v, 0);
    igraph_get_edgelist(&(this->func_g), &edges_v, false);
    igraph_integer_t ne = igraph_vector_int_size(&edges_v);
    std::vector<std::pair<int,int>> edges;
    edges.reserve(ne / 2);
    for (igraph_integer_t i = 0; i < ne; i += 2)
        edges.emplace_back((int)VECTOR(edges_v)[i], (int)VECTOR(edges_v)[i + 1]);
    igraph_vector_int_destroy(&edges_v);
    return edges;
}

std::vector<std::string> FuncTree::get_func_names() const {
    igraph_integer_t nv = igraph_vcount(&(this->func_g));
    std::vector<std::string> names;
    names.reserve(nv);
    for (igraph_integer_t i = 0; i < nv; i++)
        names.push_back(VAS(&(this->func_g), "func_name", i));
    return names;
}

void FuncTree::annotate_func_graph(){
    std::vector<bool> is_nd(igraph_vcount(&(this->evg)), false);
    std::vector<igraph_integer_t> func_node_count(igraph_vcount(&(this->func_g)), 0);
    std::vector<igraph_integer_t> func_nd_count(igraph_vcount(&(this->func_g)), 0);
    this->nd_scores.resize(igraph_vcount(&(this->func_g)));
    
    for (igraph_integer_t i = 0; i<(igraph_integer_t)(this->nd_nodes).size(); i++)
        is_nd[(this->nd_nodes)[i]] = true;
    #pragma omp parallel for
    for(igraph_integer_t i = 0; i<igraph_vcount(&(this->evg)); i++){
        int p = VECTOR(this->pid)[i];
        std::unordered_map<std::string, std::string> sym_tab = this->sym_tab_maps[p];
        std::vector<std::string>& cs = this->callstack[i];
        for (std::string& f_addr: cs){
            std::string f_name = sym_tab[f_addr];
            int ind = this->func_name_to_id_map[f_name];
            #pragma omp atomic
            func_node_count[ind]++;
            if (is_nd[i]){
                #pragma omp atomic
                func_nd_count[ind]++;
            }
        }
    }
    #pragma omp parallel for
    for (int i = 0; i<func_node_count.size(); i++){
        if (func_node_count[i]==0)
            continue;
        this->nd_scores[i] = func_nd_count[i]/func_node_count[i];
    }
}