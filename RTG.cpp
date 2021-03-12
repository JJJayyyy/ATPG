//
// Created by Jay on 10/29/20.
//

#include "circuit.hpp"
#include <string>

void circuit::RTG_simulation(int input_pattern){
    //(initial value of each node in circuit is -1)
    for (int i = 0; i < Nnodes; i++){
        np = Node[i];
        np -> val = -1;
    }

    for(int i = 0; i<Npi; i++){ // set the Pis according to the input pattern
        if((input_pattern/2)!=0)
        {
            Pinput[i]->val =  input_pattern%2;
            input_pattern/=2;
        } else{
            Pinput[i]->val =  input_pattern;
        }
        cout << "Primary num and val is "<<Pinput[i]->num<<","<<Pinput[i]->val<<endl;
    } // for

    // find the max lvl from all Poutput
    int max_lvl = 0;
    int sum = 0; //in xor gate
    for (int i = 0; i < Npo; i++){
        np = Poutput[i];
        if (np->lvl > max_lvl){
            max_lvl = np->lvl;
        }
    }
    for (int j = 1; j <= max_lvl; j++){ // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
        for(int i = 0; i < Nnodes; i++){
            np = Node[i];
            if (np->lvl == j){
                switch(np->type){
                    case (e_gtype::BRCH):
                        np->val = np->unodes[0]->val;
                        //cout<<"it's branck"<<endl;
                        break;
                    case (e_gtype::XOR):
                        sum = 0;
                        for (int k = 0 ; k < np->fin; k ++){
                            sum = sum ^ np->unodes[k]->val;
                        }
                        np->val = sum;
                        //cout<<"it's XOR"<<endl;
                        break;
                    case  (e_gtype::OR):
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 1){
                                np->val = 1;
                                break;
                            }
                            else np->val = 0;
                        } // for k
                        //cout<<"it's OR"<<endl;
                        break;

                    case  (e_gtype::NOR):
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 1){
                                np->val = 0;
                                break;
                            }
                            else np->val = 1;
                        } // for k
                        //cout<<"it's NOR"<<endl;
                        break;
                    case  (e_gtype::NOT):
                        np->val = !(np->unodes[0]->val);
                        //cout<<"it's NOT"<<endl;
                        break;
                    case (e_gtype::NAND):
                        //cout<<"it's NAND"<<endl;
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 0){
                                //cout<<"unodes:"<<np->unodes[k]->num<<": "<<np->unodes[k]->val<<endl;
                                np->val = 1;
                                break;
                            }
                            else np->val = 0;
                        } // for k
                        break;
                    case  (e_gtype::AND):
                        //cout<<"it's AND"<<endl;
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 0){
                                np->val = 0;
                                break;
                            }
                            else np->val = 1;
                        } // for k
                        break;
                    default: cout<<"it's not valid type !"<<endl;
                } // swith type
            } // if lvl = j

        } // for i < Nnodes
    }// for j < max_lvl
    this->simulation_flag = 1;
}

void circuit::RTG(string sN_tol, string sN_TFCR){
    int N_tol = stoi(sN_tol);
    int N_TFCR = stoi(sN_TFCR);
    int max;
    srand(time(NULL));
    max = 100; //set the upper bound to generate the random number
    DFS_clause RTG_clause; // create the fault set for RTG function
    if(this->read_flag == 0){
        std::cout<<"Circuit should be read before RTG!"<<std::endl;
        return;
    }
    for(int i = 1; i<=N_tol; i++) { //generate 10 random numbers for input patterns
        int random_number = rand()%max;
        this->RTG_input_pattern_set.push_back(random_number);
        cout << "The random number is: "<< this->RTG_input_pattern_set[i] << endl;
        RTG_simulation(this->RTG_input_pattern_set[i]); // do the simulation
        std::cout<<"RTG simulation finish"<<std::endl;
        DFS();
        for(int j = 0; j < Npo; j++){
            RTG_clause = RTG_clause + this->Poutput[j]->ssa_clause;
        } // add the detected faults to the RTG_clause
        if(i == 1 || i == N_tol || i%N_TFCR==0 ){
            float factor = RTG_clause.ssa1_list.size()+RTG_clause.ssa0_list.size();
            float node_size_double = Node.size()*2;
            float fault_coverage = factor/node_size_double;
            this->Fault_coverage_record.push_back(fault_coverage);
        }
    }

    std::cout<<"RTG_clause:"<<std::endl;
    for(int node_id: RTG_clause.ssa0_list)
    {
        std::cout<<node_id<<"@0"<<endl;
    }
    for(int node_id: RTG_clause.ssa1_list)
    {
        std::cout<<node_id<<"@1"<<endl;
    }
    for(int i=0; i<this->Fault_coverage_record.size(); i++){
        std::cout<< "FC: "<< Fault_coverage_record[i]<< "%"<<endl;
    }


}





