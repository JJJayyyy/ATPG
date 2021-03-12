#ifndef __DFS_SIMUILATOR_HPP__
#define __DFS_SIMUILATOR_HPP__

#include <vector>

class DFS_clause
{
private:
    

public:
    std::vector<int> ssa0_list;
    std::vector<int> ssa1_list;

    DFS_clause operator+(DFS_clause src_clause);
    DFS_clause operator-(DFS_clause src_clause);
    DFS_clause operator&(DFS_clause src_clause);
    // DFS_clause operator=(DFS_clause src_clause);

    void add_myself(int node_id, int node_val);
    void print_DFS(void);

};

#endif
