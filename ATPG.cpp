#include "circuit.hpp"
#include "time.h"
#include "ATPG.hpp"
#include "iostream"
#include <string>

//#define INCLUDE_DALG


void circuit::ATPG_PODEM_DFS(int node_id, int node_val, ofstream &ftp){
//    cout << node_id << "@" << node_val << endl;
    int PODEM_iter = 0;
    DFS_clause faults_in_DFS_ATPG;
    this->PODEM_ssa_node = node_id;
    this->PODEM_ssa_val = node_val;
    PODEM_reset();
    if(PODEM(1, PODEM_iter) == PODEM_result::PODEM_SUCCESS)
    {
        faults_in_DFS_ATPG = DFS_ATPG();
        for(int node_id: faults_in_DFS_ATPG.ssa1_list)
        {
            this->node_fault_1_map[this->num_to_idx[node_id]] = true;
            this->rfl_fault_1_map[this->num_to_idx[node_id]] = true;
        }
        for(int node_id: faults_in_DFS_ATPG.ssa0_list)
        {
            this->node_fault_0_map[this->num_to_idx[node_id]] = true;
            this->rfl_fault_0_map[this->num_to_idx[node_id]] = true;
        }
        for(int i=0;i<Npi;i++)
        {
            auto node_input = Node[num_to_idx[Pinput[i]->num]];
            if(node_input->PODEM_val_temp == LOGIC_1 || node_input->PODEM_val_temp == LOGIC_D)
                ftp << "1";
            else
                ftp << "0";

            if(i == Npi-1) 
                ftp << endl;
            else 
                ftp << ",";
        }
        // std::cout<<"=== pattern ==="<<std::endl;
        // for(auto PI_node: this->Pinput)
        // {
        //     std::cout<<" "<<PI_node->logic_5_to_char(PI_node->PODEM_val_temp);
        // }
        // std::cout<<std::endl;
        // std::cout<<"--- eliminating 0 ---"<<std::endl;
        // for(int node_num: faults_in_DFS_ATPG.ssa0_list)
        // {
        //     std::cout<<" "<<node_num;
        // }
        // std::cout<<std::endl;
        // std::cout<<"--- eliminating 1 ---"<<std::endl;
        // for(int node_num: faults_in_DFS_ATPG.ssa1_list)
        // {
        //     std::cout<<" "<<node_num;
        // }
        // std::cout<<std::endl;
    }
}

#ifdef INCLUDE_DALG
bool circuit:: ATPG_DALG(int node_num, int fault, ofstream &TP_out){
    bool DALG_RESULT;
    
    // clear circuit recovery stack
    while(!this->ckt_rec_stk.empty())
    {
        this->ckt_rec_stk.pop();
    }

    // Reset DALG_rst vector
    DALG_rst_num.clear();
    DALG_rst_val5.clear();

    // Reset all node value in circuit and set the fault
    set_ckt_val5(node_num, fault);

    // Reset DALG counter
    DALG_cnt = 0;

    if(D_ALGORITHM() == false){
        DALG_RESULT = false;
    }
    else{
        DALG_RESULT = true;
        sDALG_ATPG(node_num, fault,TP_out);
    }

    return DALG_RESULT;
}
#endif


void circuit::ATPG_PODEM_noRFL(string circuit_name)
{
    clock_t start_time, end_time;
    start_time = clock();
    circuit_name = get_cname(circuit_name);
    string f_pattern_out = circuit_name + "_PODEM_ATPG_pattern.txt";
    ofstream ftp;
    ftp.open(f_pattern_out, ios::out);
    for(int i=0;i<Npi;i++){
        auto node_individual = Node[num_to_idx[Pinput[i]->num]];
        if(i == Npi-1) ftp << node_individual->num << endl;
        else ftp << node_individual->num << ",";
    }
    for(int i = 0; i < Nnodes; i++)
    {  // initialize the faults are not detected by RFL
        node_fault_1_map[i] = false;
        node_fault_0_map[i] = false;
    }
    int index = 0;
    node* node_under_test;
    for(int i = 0; i<Nnodes; i++){ // ATPG
        node_under_test = Node[i];
        if(!node_fault_1_map[i]){
            ATPG_PODEM_DFS(node_under_test->num, 1, ftp);
        }
        if(!node_fault_0_map[i]){
            ATPG_PODEM_DFS(node_under_test->num, 0, ftp);
        }
    }

    end_time = clock();
    float factor = 0;
    for(int i = 0; i<Nnodes; i++){
        if(node_fault_1_map[i]) factor+=1;
        if(node_fault_0_map[i]) factor+=1;
    }
    float node_size_double = Node.size()*2;
    float fault_coverage = factor/node_size_double;
    double period = double(end_time - start_time)/CLOCKS_PER_SEC;
//    cout << "time: " << period << endl;
//    cout << "fault_coverage: " << fault_coverage << endl;
    string fout = circuit_name + "_PODEM_ATPG_report.txt";
    fp.open(fout, ios::out);
    fp<<setiosflags(ios::fixed)<<setprecision(2);
    fp << "Algorithm: PODEM" << endl;
    fp << "Circuit: " << circuit_name << endl;
    fp << "Fault Coverage: " << fault_coverage*100 << "%" << endl;
    fp << "Time: " << period << " Seconds" << endl;
    ftp.close();
    fp.close();
}
