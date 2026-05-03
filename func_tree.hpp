#include <vector>
#include <unordered_map>
#include <set>
#include <omp.h>
#include <iostream>

extern "C" {
#include <igraph.h>
#include <igraph_attributes.h>
}

class FuncTree {
    public:
    FuncTree(std::string graph_path ,std::vector<std::unordered_map<std::string, std::string>> sym_tab_maps, int num_proc, std::vector<igraph_integer_t> nd_nodes): num_proc(num_proc), sym_tab_maps(sym_tab_maps), nd_nodes(nd_nodes){
        igraph_t g;
        igraph_set_attribute_table(&igraph_cattribute_table);
        FILE* f = fopen(graph_path.c_str(), "r");
        if (!f)
            throw std::runtime_error("Cannot open input file: " + graph_path);
        int err = igraph_read_graph_graphml(&g, f, 0);
        fclose(f);
        if (err != IGRAPH_SUCCESS)
            throw std::runtime_error("Error reading GraphML: " + graph_path);
        this->evg = g;
        igraph_vector_init(&(this->pid), 0);
        igraph_strvector_init(&(this->attr_callstack), 0);
        VASV(&g, "callstack",&(this->attr_callstack));
        VANV(&g, "process_id", &(this->pid));
        this->callstack.resize(igraph_vcount(&g));
        std::cout<<"Constructor checkpoint 1"<<std::endl;
        set_callstack();
        std::cout<<"Constructor checkpoint 2"<<std::endl;
        create_func_graph();
        std::cout<<"Constructor checkpoint 3"<<std::endl;
        annotate_func_graph();
        std::cout<<"Constructor checkpoint 4"<<std::endl;
    }

    ~FuncTree(){
        igraph_destroy(&evg);
        igraph_destroy(&func_g);
        igraph_vector_destroy(&pid);
        igraph_strvector_destroy(&attr_callstack);
    }

    void set_callstack();
    void create_func_graph();
    void annotate_func_graph();

    std::vector<std::pair<int,int>> get_edge_list() const;
    std::vector<std::string> get_func_names() const;
    std::vector<double> get_nd_scores() const { return nd_scores; }




    private:
    igraph_t evg, func_g;
    igraph_vector_t pid;
    igraph_strvector_t attr_callstack;
    std::vector<std::vector<std::string>> callstack;
    std::vector<igraph_integer_t> nd_nodes;
    int num_proc;
    std::vector<std::unordered_map<std::string, std::string>> sym_tab_maps;
    std::vector<double> nd_scores;
    std::unordered_map<std::string, int> func_name_to_id_map;
};