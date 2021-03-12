

#include "circuit.hpp"


void circuit::PFS_reset(void)
{   
    this->PFS_temp_mask = 0x01;
#if ULLLONG_MAX == 0xffffffffffffffffffffffffffffffff
    PFS_parallel_num = 128;
#elif ULLONG_MAX == 0xffffffffffffffff
    PFS_parallel_num = 64;
#elif ULONG_MAX == 0xffffffff
    PFS_parallel_num = 32;
#else
    PFS_parallel_num = 16;
#endif
    for(auto x: this->Node)
    {
        x->PFS_mask.ssa0_mask = this->PFS_max_val;
        x->PFS_mask.ssa1_mask = 0;
    }
    this->PFS_fault_exist.clear();
    this->PFS_ssa_location.clear();
    this->PFS_ssa_value.clear();
    this->PFS_ssa1_result.clear();
    this->PFS_ssa0_result.clear();
}

inline void circuit::PFS_reset_mask(void)
{
    for(auto x: this->Node)
    {
        x->PFS_mask.ssa0_mask = this->PFS_max_val;
        x->PFS_mask.ssa1_mask = 0;
    }
}

inline void circuit::PFS_reset_fault_exist_map(void)
{
    this->PFS_fault_exist.clear();
}

inline void circuit::PFS_reset_ssa_list(void)
{
    this->PFS_ssa_location.clear();
    this->PFS_ssa_value.clear();
}

int circuit::PFS_set_ssa(int node_id, int node_val, int mask_idx)
{
    // if(this->PFS_ssa_location.size() == (this->PFS_parallel_num - 1))
    // {
    //     std::cout<<"overflow: printing when setting "<<node_id<<" for value "<<node_val<<std::endl;
    //     // std::cout<<"max simulation is "<<this->PFS_parallel_num<<" and faults for this iteration is full"<<std::endl;
    //     return 0;
    // }
    // this->PFS_ssa_location.push_back(node_id);
    // this->PFS_ssa_value.push_back(node_val);
    this->PFS_fault_exist[node_id] = 1;
    for(auto search_node: this->Node)
    {
        if(search_node->num == node_id)
        {
            if(node_val == 0)
            {
                search_node->PFS_mask.ssa0_mask = (~(this->PFS_temp_mask << mask_idx)) & search_node->PFS_mask.ssa0_mask;   // set the corresponding bit to ssa0
            }
            else
            {
                search_node->PFS_mask.ssa1_mask = (this->PFS_temp_mask << mask_idx) | search_node->PFS_mask.ssa1_mask;      // set the corresponding bit to ssa1
            }
            break;
        }
    }
    return 1;
}

void circuit::PFS_load_input(string input_file_name)
{
    char temp;
    fin.open(input_file_name, ios::in);
    
    while (getline(fin, log_in)) {
        stringstream col(log_in);
       
        col>>log_num;
        col>>temp;
        col>>log_val;
       // cout<<"log_num = "<<log_num<<endl;
        for(int i = 0; i < Npi; i++){
            auto PI_node = Pinput[i];
            //cout<<"num = "<<PI_node->num<<endl;
            if(PI_node->num == log_num){
                if(log_val == 1)
                {
                    PI_node->PFS_val = this->PFS_max_val;
                }
                else
                {
                    PI_node->PFS_val = 0;
                }
                break;
               // cout << "Primary num and val is "<<PI_node->num<<","<<PI_node->val<<endl;
            }// if read file line successful
        } // for
    } // while
    fin.close();
}


void circuit::PFS_load_fault(string input_file_name)
{
    char char_temp;
    int ssa_location_temp;
    int ssa_value_temp;
    fin.open(input_file_name, ios::in);

    this->PFS_ssa_location.clear();
    this->PFS_ssa_value.clear();
    
    while (getline(fin, log_in)) 
    {
        stringstream col(log_in);

        
        col>>ssa_location_temp;
        col>>char_temp;
        col>>ssa_value_temp;
        // cout<<"log_num = "<<log_num<<endl;
        this->PFS_ssa_location.push_back(ssa_location_temp);
        this->PFS_ssa_value.push_back(ssa_value_temp);
    } // while
    fin.close();
}

void circuit::PFS_simulate(void)
{
    auto sum = this->PFS_max_val; //in xor gate

    for(auto PI_node: this->Pinput)
    {
        if(this->PFS_fault_exist[PI_node->num] == 1)
        {
            PI_node->PFS_val = (PI_node->PFS_val & PI_node->PFS_mask.ssa0_mask) | PI_node->PFS_mask.ssa1_mask;
        }
    }
    
    // for all node which lvl is j (from 1 to max lvl)
        // if type = 1 (branch)  val = unodes[0] -> val
        // if type = 6 (NAND)  if undoes[i(from 0 to fin-1)]=0, val = 1, otherwise val =0 (control value = 0)
        // ...... other type
    for (int j = 1; j <= this->max_lvl; j++){ // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
        for(auto x : this->table[j]){
          
                switch(x->type){
                        
                    case (e_gtype::BRCH):
                        x->PFS_val = x->unodes[0]->PFS_val;
                        //cout<<"it's branck"<<endl;
                        break;
                    
                    case (e_gtype::XOR):
                        sum = 0;
                        for (int k = 0 ; k < x->fin; k ++)
                        {
                            sum = sum ^ x->unodes[k]->PFS_val;
                        }
                        x->PFS_val = sum;
                        //cout<<"it's XOR"<<endl;
                        break;
                        
                    case (e_gtype::OR):
                        sum = 0;
                        for (int k = 0 ; k < x->fin; k ++)
                        {
                            sum = sum | x->unodes[k]->PFS_val;
                        }
                        x->PFS_val = sum;
                        //cout<<"it's OR"<<endl;
                        break;
                        
                    case (e_gtype::NOR):
                        sum = 0;
                        for (int k = 0 ; k < x->fin; k ++)
                        {
                            sum = sum | x->unodes[k]->PFS_val;
                        }
                        x->PFS_val = ~sum;
                        //cout<<"it's NOR"<<endl;
                        break;
                    
                    case (e_gtype::NOT):
                        x->PFS_val = ~(x->unodes[0]->PFS_val);
                        //cout<<"it's NOT"<<endl;
                        break;
       
                    case (e_gtype::NAND):
                        sum = this->PFS_max_val;
                        for (int k = 0 ; k < x->fin; k ++)
                        {
                            sum = sum & x->unodes[k]->PFS_val;
                        }
                        x->PFS_val = ~sum;
                        break;

                    
                    case  (e_gtype::AND):
                        sum = this->PFS_max_val;
                        for (int k = 0 ; k < x->fin; k ++)
                        {
                            sum = sum & x->unodes[k]->PFS_val;
                        }
                        x->PFS_val = sum;
                        break;
                    
                    default: 
                        cout<<"it's not valid type !"<<endl;
                        break; 
                } // swith type

            if(this->PFS_fault_exist[x->num] == 1)
            {
                x->PFS_val = (x->PFS_val & x->PFS_mask.ssa0_mask) | x->PFS_mask.ssa1_mask;
            }
            
        } // for i < Nnodes
    }// for j < max_lvl
    std::cout<<"-------------- PFS Simulation End --------------"<<std::endl;
    this->simulation_flag = 1;

}

// void circuit::PFS_extract_result(void)
// {
//     auto mask = this->PFS_temp_mask;
//     int max_iter_num = this->PFS_ssa_location.size();
//     int golden_val;
//     for(auto PO_node: this->Poutput)
//     {
//         golden_val = PO_node->PFS_val & mask;
//         for(int i = 1; i <= max_iter_num; i++)
//         {
//             if(((PO_node->PFS_val & (mask << i)) >> i) != golden_val)
//             {
//                 if(this->PFS_ssa_value[i-1] == 1)
//                 {
//                     this->PFS_ssa1_result[this->PFS_ssa_location[i-1]] = 1;
//                 }
//                 else
//                 {
//                     this->PFS_ssa0_result[this->PFS_ssa_location[i-1]] = 1;
//                 }
                
//             }
//         }
//     }
// }


void circuit::PFS_continuous(string input_file_name, string fault_file_name, string output_file_name)
{
    auto mask = this->PFS_temp_mask;
    int golden_val;
    this->PFS_reset();
    std::vector<std::vector<char>> TP_vector = this->log_read_TP(input_file_name);
    this->PFS_load_fault(fault_file_name);
    int fault_cnt = this->PFS_ssa_location.size();
    int simulation_iter;
    simulation_iter = fault_cnt /(this->PFS_parallel_num - 1);

    if((fault_cnt % (this->PFS_parallel_num - 1)) != 0)

    {
        simulation_iter = simulation_iter + 1;
    }

    char logic_bit;

    int parallel_in_this_iter;
    int max_parallel_num = this->PFS_parallel_num - 1;

    for(auto TP: TP_vector)
    {
        for(int i = 0; i < simulation_iter; i++)
        {
            for(node* PI_node: Pinput)
            {
                logic_bit = TP[PI_node->TP_idx];
                if(logic_bit == '1')
                {
                    PI_node->PFS_val = this->PFS_max_val;
                }
                else if(logic_bit = '0')
                {
                    PI_node->PFS_val = 0;
                }
            }

            this->PFS_reset_mask();
            // this->PFS_reset_ssa_list();
            this->PFS_reset_fault_exist_map();

            // parallel_in_this_iter = this->PFS_ssa_location.size() - (max_parallel_num) * i;
            // if(parallel_in_this_iter >= max_parallel_num)
            // {
            //     parallel_in_this_iter = max_parallel_num;
            // }

            if(i != (simulation_iter - 1))
            {
                parallel_in_this_iter = max_parallel_num;
            }
            else
            {
                parallel_in_this_iter = this->PFS_ssa_location.size() % max_parallel_num;
            }
            for(int j = 0; j < parallel_in_this_iter; j++)
            {
                this->PFS_set_ssa(this->PFS_ssa_location[i * max_parallel_num + j], this->PFS_ssa_value[i * max_parallel_num + j], j + 1);
            }

            this->PFS_simulate();

            for(auto PO_node: this->Poutput)
            {
                golden_val = PO_node->PFS_val & mask;
                for(int j = 0; j < parallel_in_this_iter; j++)
                {
                    if(((PO_node->PFS_val & (mask << (j + 1))) >> (j + 1)) != golden_val)
                    {
                        if(this->PFS_ssa_value[i * max_parallel_num + j] == 1)
                        {
                            std::cout<<"node "<<this->PFS_ssa_location[i * max_parallel_num + j]<<" ssa1 is detectable"<<std::endl;
                            this->PFS_ssa1_result[this->PFS_ssa_location[i * max_parallel_num + j]] = 1;
                        }
                        else
                        {
                            std::cout<<"node "<<this->PFS_ssa_location[i * max_parallel_num + j]<<" ssa0 is detectable"<<std::endl;
                            this->PFS_ssa0_result[this->PFS_ssa_location[i * max_parallel_num + j]] = 1;
                        }
                        
                    }
                }
            }

        }
    }

    fp.open(output_file_name, ios::out);

    for(auto iter: this->PFS_ssa0_result)
    {
        if(iter.second != 0)
        {
            fp<<iter.first<<"@0"<<endl;
        }
    }

    for(auto iter: this->PFS_ssa1_result)
    {
        if(iter.second != 0)
        {
            fp<<iter.first<<"@1"<<endl;
        }
    }

    fp.close();

}

