#include <vector>
#include <string>


std::vector<std::string> get_callstack_vec(const std::string& callstack_str){
    std::vector<std::string> callstack_vec;
    std::string curr_func = "";
    for (auto c: callstack_str){
        if (c==' '||c=='\n') continue;
        if (c==','){
            callstack_vec.push_back(curr_func);
            curr_func = "";
            continue;
        }
        curr_func += c;
    }
    if (curr_func.size()>0)
        callstack_vec.push_back(curr_func);
    return callstack_vec;
}