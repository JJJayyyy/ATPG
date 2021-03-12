#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "DFS_simulator.hpp"
#include "PFS_simulator.hpp"


#include <iostream>
#include <string>
#include <vector>


#define LOGIC_0 0x00
#define LOGIC_1 0xFF
#define LOGIC_D 0x0F
#define LOGIC_Dn 0xF0
#define LOGIC_X 0xC3
#define LOGIC_XX 0x3C


using namespace std;

enum class e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND}; 
enum class e_ntype {GATE, PI, FB, PO};

// enum class logic_5_typedef {LOGIC_0, LOGIC_1, LOGIC_X, LOGIC_D, LOGIC_Dn};


class node{

public:
    int idx;
    int num;
    e_gtype type; // e.g. Node.type = e_gtype::IPT
    int type_num;
    int fin;
    int fout;
    int lvl;
    e_ntype node_type;
    int TP_idx; // Test Pattern column num for logic sim
    bool XOR_D_fnter_c_cbar = false; // indicator to check if have tried to propagate XOR D_fnter with both c and c_bar
    vector <node*> unodes;
    vector <node*> dnodes;

    DFS_clause ssa_clause;

    uint8_t value_5;

    char logic_5_to_char(uint8_t logic_5_in);

    //~node();
    void pn();

    int rfl_0;
    int rfl_1;
    int val;

    PFS_mask_handler PFS_mask;

#if ULLLONG_MAX == 0xffffffffffffffffffffffffffffffff
    uint128_t PFS_val;
#elif ULLONG_MAX == 0xffffffffffffffff
    uint64_t PFS_val;
#elif ULONG_MAX == 0xffffffff
    uint32_t PFS_val;
#else
    uint16_t PFS_val;
#endif

    /******* start public definition for ATPG *******/
    uint8_t InC_val_fixed;
    uint8_t InC_val_temp;
    /*******  end public definition for ATPG  *******/


    /******* start public definition for ATPG *******/
    uint8_t PODEM_val_fixed;
    uint8_t PODEM_val_temp;

    int PODEM_backtrace_check_flag = 0;
    /*******  end public definition for ATPG  *******/

    // void deepCopy(const node& src);
    

};

#endif