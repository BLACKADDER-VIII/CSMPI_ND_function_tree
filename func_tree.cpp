#include "func_tree.hpp"
#include "callstack_parse_utilities.cpp"


FuncTree::set_callstack(){
    for (igraph_integer_t i = 0; i<igraph_strvector_size(&(this->attr_callstack)); i++){
        this->callstack[i] = get_callstack_vec(igraph_strvector_at(&(this->attr_callstack), i));
    }
}