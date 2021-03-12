#ifndef __DALG_HPP__
#define __DALG_HPP__

// #define DFS_CHECK_FOR_ATPG
// #define DALG_DEBUG_INFO_ON
#define DALG_FIRST_COEFF 10
#define DALG_SECOND_COEFF 3
#define DALG_THIRD_COEFF 2
#define DALG_FOURTH_COEFF 1
#define DALG_TO_LIMIT_1 1000
#define DALG_TO_LIMIT_2 2000
#define DALG_TO_LIMIT_3 5000
#define RFL_REDUCTION_FOR_DALG
#define DFS_REDUCTION_FOR_DALG

class ckt_snapsht{
public:
    // copy of all nodes' value 5
    // key: node->num, value: node->value_5
    unordered_map<int, uint8_t> node_val5_copy;
    map<node*, int> D_fnter_copy, J_fnter_copy;
};


#endif