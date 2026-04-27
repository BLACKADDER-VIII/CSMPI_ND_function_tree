#include <vector>
#include <unordered_map>

extern "C" {
#include <igraph.h>
#include <igraph_attributes.h>
}

class FuncTree {
    public:
    FuncTree(std::string graph_path ,std::vector<std::unordered_map<std::string, std::string>> sym_tab_maps, int num_proc): num_proc(num_proc), sym_tab_maps(sym_tab_maps){
        igraph_t g;
        igraph_set_attribute_table(&igraph_cattribute_table);
        FILE* f = fopen(graph_path.c_str(), "r");
        if (!f)
            throw std::runtime_error("Cannot open input file: " + path);
        int err = igraph_read_graph_graphml(&g, f, 0);
        fclose(f);
        if (err != IGRAPH_SUCCESS)
            throw std::runtime_error("Error reading GraphML: " + path);
        this->evg = g;
        VASV(&g, "callstack",&(this->attr_callstack));
        VANV(&g, "process_id", &(this->pid));
        this->callstack.resize(g.vcount());
        set_callstack();
    }

    void set_callstack();
    




    private:
    igraph_t evg, func_g;
    igraph_vector_t pid;
    igraph_strvector_t attr_callstack;
    std::vector<std::vector<std::string>> callstack;


};