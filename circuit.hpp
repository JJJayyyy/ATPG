#ifndef __CIRCUIT_HPP__
#define __CIRCUIT_HPP__

#include <bits/stdc++.h>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <list>
#include "node.hpp"
#include "PODEM.hpp"
#include "DALG.hpp"
#include "ATPG.hpp"

using namespace std;

class circuit{

private:
    /*Variables for read*/
    int ntbl, *tbl, nd, tp, fo, fi, ni = 0, no = 0;
    ifstream fin;
    

    /*Function for read*/
    string get_gname(int tp);
    
    
    /*Variables for lev*/
    int lvl_done, fin_max, fin_cur, fin_ok = 0;
    node* np;
    string file_name;
    
    /*Variables for log*/
    int log_num, log_val;
    string log_in;
    vector<vector<char>> log_sim_5_result;
    
    fstream fp;

    int max_lvl;
    list<node*> *table;
    
    int read_flag = 0;
    int levelization_flag = 0;
    int simulation_flag = 0;


    /******* start private definition for ATPG *******/
    vector<node*> InC_vector;       // vector for Imply-and-Check/Imply. Need to be cleared after each InC/Imply recursion.
                                    // Content are nodes that need InC
    map<int, int> InC_map;          // map for saving all existance status in this->InC_vector

    /*******  end private definition for ATPG  *******/

    /******* start private definition for PODEM *******/
    
    /*******  end private definition for PODEM  *******/

    
public:
    int Nnodes, Npi, Npo, NGates;
    vector<node*> Node, Pinput, Poutput;
    std::map<int, int> num_to_idx;

    circuit(string ckt_file);
    //~circuit();

    void read(string file_name);

    void lev(); // do levelization

    string get_cname(string ckt_fin);

    /*I/O functions*/
    void pc();
    void plev(string ckt_fin); // print levelization info onto terminal
    void slev(string slev_fout, string ckt_fin); // save levelization info into file
    
    void log_sim(string log_file_name);
    void log_sim(void);
    void log_sim_5(string log_file_name);
    vector<vector<char>> log_read_TP(string test_in);

    
    void plog(); // print logic simulation info on terminal
    void slog(string log_output_file); // save logic simulation info in txt
    void plog(int PO_header);
    void slog(int PO_header, string output_file);

    void sDALG(int node_num, int fault, string ckt_fin);
    void sPODEM(string ckt_fin);

    int read_done(void);
    int lev_done(void);
    int sim_done(void);

    void RFL(string output_file_name);
    void RFL_reduce();
    void RTG(string sN_tol, string sN_TFCR);
    void RTG_test_pattern_report(string output_file_name);
    void RTG_FC_report(string sN_tol, string sN_TFCR, string output_file_name);
    void RTG_simulation(int input_pattern);

    std::vector<int>RTG_input_pattern_set;
    std::vector<float>Fault_coverage_record;

    void PFS();
    void DFS(void);

    void sDFS_once(string output_file_name);
    void DFS_continuous(string input_file_name, string output_file_name);


    //string get_cname(string ckt_fin);


    /******* start public PV definition for PFS *******/
    std::vector<int> PFS_ssa_location;
    std::vector<int> PFS_ssa_value;
    std::map<int,int> PFS_fault_exist;
    std::map<int,int> PFS_ssa0_result, PFS_ssa1_result;

#if ULLLONG_MAX == 0xffffffffffffffffffffffffffffffff
    uint128_t PFS_temp_mask = 0x01;
    int PFS_parallel_num = 128;
    uint128_t PFS_max_val = ULLLONG_MAX;
#elif ULLONG_MAX == 0xffffffffffffffff
    uint64_t PFS_temp_mask = 0x01;
    int PFS_parallel_num = 64;
    uint64_t PFS_max_val = ULLONG_MAX;
#elif ULONG_MAX == 0xffffffff
    uint32_t PFS_temp_mask = 0x01;
    int PFS_parallel_num = 32;
    uint32_t PFS_max_val = ULONG_MAX;
#else
    uint16_t PFS_temp_mask = 0x01;
    int PFS_parallel_num = 16;
    uint16_t PFS_max_val = 0xFFFF;
#endif

    void PFS_reset(void);
    inline void PFS_reset_mask(void);
    inline void PFS_reset_fault_exist_map(void);
    inline void PFS_reset_ssa_list(void);
    int PFS_set_ssa(int node_id, int node_val, int mask_idx);
    void PFS_load_input(string input_file_name);
    void PFS_load_fault(string input_file_name);
    void PFS_simulate(void);
    void PFS_extract_result(void);
    void sPFS_once(string output_file_name);
    void PFS_continuous(string input_file_name, string fault_file_name, string output_file_name);


    /******** end public PV definition for PFS ********/

    /******* start public definition for ATPG *******/


    /*******  end public definition for ATPG  *******/



    /******* start public definition for PODEM *******/
    int PODEM_ssa_node;
    int PODEM_ssa_val;

    int PODEM_max_iter;

    map<int, int> PODEM_coverage_map_0;
    map<int, int> PODEM_coverage_map_1;


    vector<node*> PODEM_Imply_vector;
    map<int, int> PODEM_Imply_map;

    std::vector<node*> PODEM_D_Frontier;
    std::vector<node*> PODEM_J_Frontier;
    std::map<int, int> PODEM_D_Frontier_map;
    std::map<int, int> PODEM_J_Frontier_map;

    std::map<int, uint8_t> PODEM_fixed_value_map;

    void PODEM_reset(void);
    InC_result PODEM_imply(void);
    InC_result PODEM_imply(std::map<int, uint8_t> known_set);
    InC_result PODEM_fwd_imply(void);

    int PODEM_error_at_PO(void);

    void PODEM_get_D_Frontier(int fault_location);
    void PODEM_get_J_Frontier(int fault_location);


    PODEM_result PODEM(int PODEM_initial, int &iter_num);
    std::pair<int, uint8_t> PODEM_backtrace_non_DFS(std::pair<int, uint8_t> obj_result);
    std::pair<int, uint8_t> PODEM_objective(void);
    void PODEM_set_fixed_val(std::pair<int, uint8_t> obj_pair);
    void dump_all_nodes();

    //PODEM backtrace
    vector<vector<int>> path_matrix;
    pair<int, uint8_t> PODEM_backtrace(std::pair<int, uint8_t> obj_result);
    pair<int, int> PODEM_find_inversion_fault_to_PI(int fault_node_num);
    void PODEM_find_all_path();
    void PODEM_PI_path(int, bool[], vector<int>&);

    void PODEM_save_report(double run_time, string ckt_fin);
    /*******  end public definition for PODEM  *******/

/*******  start definition for D_alg  *******/

private:

    bool conflict = false;
    stack <node*> ImpnC_stk; // Foward & backward stack for doing forward imply
    map <node*, int> D_fnter, J_fnter; // key: node pointer, value: 0->untried nodes, 1->tried nodes
    vector <int> DALG_rst_num;
    vector <uint8_t> DALG_rst_val5;

public:

    int DALG_TO_coeff = 1; // Timeout coefficient for DALG. DALG will run (Nnodes*DALG_TO_coeff) repeatation

    int DALG_ssa_node, DALG_ssa_val;

    DFS_clause DFS_ATPG_DALG();
    int DALG_cnt = 0;

    stack <ckt_snapsht*> ckt_rec_stk; 
    
    map<int, int> DALG_coverage_map_0;
    map<int, int> DALG_coverage_map_1;


    node* operator << (uint8_t new_val5);
    void set_ckt_val5(int node_num, int fault); // setup fault node and initialize all other nodes' val5 to X
    void UpdnC_val5 (node* np, uint8_t new_val5);
    void push_stk (node* np);
    bool ImpnC();
    void Find_D_J_fnter();
    bool err_at_PO();
    void save_to_stk();
    void rec_from_stk();
    bool all_D_fnter_tried();
    void assign_cbar_to_D_fnter_fin(node* curr_D_fnter);
    bool all_inputs_of_curr_J_fnter_specified(node* curr_J_fnter);
    void get_c_cbar_for_curr_J_fnter(node* curr_J_fnter, uint8_t &c, uint8_t &c_bar);
    void assign_c_to_sel_in(node* sel_in, uint8_t c);
    void save_DALG_rst(vector<node*> Pinput);
    void DALG_reset();

    bool DALG(int node_num, int fault, string ckt_fin, ofstream& dpf, int one_for_single_pattern_zero_for_multi_pattern); // set fault in circuit, call D_ALGORITHM from within
    bool D_ALGORITHM();
    void DALG_save_report(double run_time, string ckt_fin);
    void sDALG_ATPG_pattern(ofstream& dpf);

    void set_DALG_timeout_coefficient();

/*******  end definition for D_alg  *******/



/*******  start definition for ATPG  *******/
bool ATPG_rst_check(string TP_in, string sDFS_fout, int node_num, int fault_in_test);
/*******  end definition for ATPG  *******/


/*******  start definition for ATPG  *******/
    map<int, bool> node_fault_1_map;
    map<int, bool> node_fault_0_map;
    map<int, bool> rfl_fault_1_map;
    map<int, bool> rfl_fault_0_map;
    DFS_clause DFS_ATPG();
    void ATPG_PODEM_DFS(int node, int val, ofstream& ftp);
    void ATPG_PODEM(string circuit_name);
    void ATPG_PODEM_2(string circuit_name);
    void ATPG_PODEM_noRFL(string circuit_name);

    void sDALG_ATPG(int node_num, int fault, ofstream &TP_out);
    bool ATPG_DALG(int node_num, int fault, ofstream &TP_out);

    float ATPG_RTG_total;
    float ATPG_RTG_current_max_avg;

};

#endif

