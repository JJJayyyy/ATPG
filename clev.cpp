#include "circuit.hpp"

void circuit:: lev(){
    // initialize all level of nodes to -1
    for(int i=0;i< Nnodes;i++){
        np = Node[i];
        np->lvl=-1;
    }
    // change all primary inputs to level 0
    for(int i=0;i< Npi;i++){
    np = Pinput[i];
    np->lvl=0;
    }
    // let levelized node count equal to number of primary inputs
    lvl_done=Npi;
    // while loop stops when all nodes are levelized
    while(1){
        for(int i=0;i< Nnodes;i++){
            fin_max=0;
            np = Node[i];
            
            fin_ok = 0; // count number of upstream nodes already levelized
            // skip all circuit alreay leveled
            if (np->lvl>=0)
                continue;
            // search for upstream nodes of np to find the max_lvl of fan_in
            for(int j=0;j<np->fin;j++){
                fin_cur=(np->unodes[j]->lvl);
                if(fin_cur>=0){
                    fin_ok++;
                    if(fin_cur>fin_max){
                        fin_max=fin_cur;
                    }
                }
            }
            // if all fan-in are leveled, than np->level=max(fan-in level)+1
            if(fin_ok==np->fin){
                np->lvl=fin_max+1;
                lvl_done++;
            }

            if(lvl_done>= Nnodes){
                break;
            }
        }
        if(lvl_done>= Nnodes){
            break;
        }
    }

    for (int i = 0; i < Npo; i++)
    {
        np = Poutput[i];
        if (np->lvl > this->max_lvl){
            this->max_lvl = np->lvl;
        }
    }
    //cout<<"max_lvl = "<<max_lvl<<endl;
       
    //build a hash table, index is level, key is num;
    this->table = new list<node*>[this->max_lvl+1];
    
    for(int i = 0; i < Nnodes; i++){
        np = Node[i];
        this->table[np->lvl].push_back(np);
        this->num_to_idx[np->num] = i;
        PODEM_coverage_map_0[np->num] = 0;
        PODEM_coverage_map_1[np->num] = 0;

    }

    this->PODEM_max_iter = this->Nnodes;

    this->levelization_flag = 1;
}
