//
// Created by Jay on 12/2/20.
//
#include "circuit.hpp"
#include "time.h"
#include "ATPG.hpp"
#include "iostream"
#include <string>
//#define INCLUDE_DALG

void circuit::ATPG_PODEM_2(string circuit_name)
{
    clock_t start_time, end_time;
    start_time = clock();
    RFL_reduce();
    circuit_name = get_cname(circuit_name);
    string f_pattern_out = circuit_name + "_ATPG_patterns.txt";
    ofstream ftp;
    ftp.open(f_pattern_out, ios::out);
    for(int i=0;i<Npi;i++){  // node print
        auto PI_node = Node[num_to_idx[Pinput[i]->num]];
        if(i == Npi-1) ftp << PI_node->num << endl;
        else ftp << PI_node->num << ",";
    }

    // RFL
    for(int i = 0; i < Nnodes; i++){  // initialize the faults are not detected by RFL
        auto node_np = Node[i];
        rfl_fault_1_map[i] = false;
        rfl_fault_0_map[i] = false;
        node_fault_1_map[i] = false;
        node_fault_0_map[i] = false;
        if(node_np->rfl_1 == 0){ // trim the nodes by rfl algorithm
            rfl_fault_1_map[i] = true;
        }
        if(node_np->rfl_0 == 0){
            rfl_fault_0_map[i] = true;
        }
    }

    // RTG
    srand(time(NULL));
    int tp_count = 0;
    bool flag = true;
    ATPG_RTG_current_max_avg = 0.0f;
    while (flag) {  //generate Nnodes/20 random input patterns
        DFS_clause overall_fault;
        for(int i=0;i<Npi;i++)  // set the PI according to the input pattern
        {
            int random_number = rand()%2;
            auto np_pi = Pinput[i];
            np_pi->val =  random_number;
            ftp << np_pi->val;     // write to the pattern file
            if(i == Npi-1) ftp << endl;
            else ftp << ",";
//            cout << "Primary num and val is "<< np->num <<","<< np->val << endl;
        }
        this->log_sim();
        this->DFS();    // find other faults with DFS
        for(node *node_po: Poutput){ // mark the detected faults
            overall_fault = overall_fault + node_po->ssa_clause;
        }
        for(int node_id: overall_fault.ssa1_list)
        {
            this->node_fault_1_map[this->num_to_idx[node_id]] = true;
            this->rfl_fault_1_map[this->num_to_idx[node_id]] = true;
        }
        for(int node_id: overall_fault.ssa0_list)
        {
            this->node_fault_0_map[this->num_to_idx[node_id]] = true;
            this->rfl_fault_0_map[this->num_to_idx[node_id]] = true;
        }
        float fault_total = 0;
        for(int i = 0; i<Nnodes; i++)
        {
            if(node_fault_1_map[i]) fault_total+=1; // if this S@1 is detected, increment counter
            if(node_fault_0_map[i]) fault_total+=1; // if this S@0 is detected, increment counter
        }
        tp_count += 1;
        // if (tp_count >= (this->Nnodes/20))  flag = false;  // end condition of the RTG
        ATPG_RTG_total = fault_total;
        float avg = ATPG_RTG_total/tp_count;
        float cov = ATPG_RTG_total / (double) (this->Nnodes * 2);
        cout << "total fault detect: "<< ATPG_RTG_total <<"; Avg: "<< avg << " and Cov: "<<cov<<endl;
        if (avg > ATPG_RTG_current_max_avg) ATPG_RTG_current_max_avg = avg;
        if(tp_count < (this->Npi * 0.5))
            continue;
        if(avg < (ATPG_RTG_current_max_avg*0.05)) 
            flag = false;  // end condition of the RTG
        if(cov > 0.75f)
            flag = false;
    }


    // ATPG
    int index = 0;
    for(int i = 0; i<Nnodes; i++){
        auto node_np = Node[i];
        if(!rfl_fault_1_map[i] && !node_fault_1_map[i])
        { // check the remaining nodes after rfl
            ATPG_PODEM_DFS(node_np->num, 1, ftp);
        }
        if(!rfl_fault_0_map[i] && !node_fault_0_map[i])
        {
            ATPG_PODEM_DFS(node_np->num, 0, ftp);
        }
    }
    for(int i = 0; i<Nnodes; i++)
    {
        auto node_np = Node[i];
        if(rfl_fault_1_map[i] && !node_fault_1_map[i])
            ATPG_PODEM_DFS(node_np->num, 1, ftp);
        if(rfl_fault_0_map[i] && !node_fault_0_map[i])
            ATPG_PODEM_DFS(node_np->num, 0, ftp);
    }

#ifdef INCLUDE_DALG
    for(int i = 0; i<Nnodes; i++){ // ATPG
        np = Node[i];
        if(!node_fault_1_map[i]){
            if(ATPG_DALG(np->num, 1, ftp))
            {
                node_fault_1_map[i] = true;
            }
        }
        if(!node_fault_0_map[i]){
            if(ATPG_DALG(np->num, 1, ftp))
            {
                node_fault_0_map[i] = true;
            }
        }
    }
#endif

    end_time = clock();
    float factor = 0;
    for(int i = 0; i<Nnodes; i++)
    {
        if(node_fault_1_map[i]) factor+=1; // if this S@1 is detected, increment counter
        if(node_fault_0_map[i]) factor+=1; // if this S@0 is detected, increment counter
    }
    
    float node_size_double = this->Nnodes*2;
    float fault_coverage = factor/node_size_double;
    double period = double(end_time - start_time)/CLOCKS_PER_SEC;
//    cout << "time: " << period << endl;
//    cout << "fault_coverage: " << fault_coverage << endl;
    string fout = circuit_name + "_ATPG_report.txt";
    fp.open(fout, ios::out);
    fp<<setiosflags(ios::fixed)<<setprecision(2);
#ifdef INCLUDE_DALG
    fp << "ATPG PODEM & DALG" << endl;
#else
    fp << "Algorithm: ATPG" << endl;
#endif
    fp << "Circuit: " << circuit_name << endl;
    fp << "Fault Coverage: " << fault_coverage*100 << "%" << endl;
    fp << "Time: " << period << " seconds" << endl;
    ftp.close();
    fp.close();
}









