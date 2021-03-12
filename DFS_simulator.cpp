// #include "DFS_simulator.hpp"
// #include "node.hpp"
// #include "circuit.hpp"

// #include <map>
// #include <algorithm>
// #include <iostream>

using namespace std;

// DFS_clause DFS_clause::operator=(DFS_clause src_clause)
// {
//     DFS_clause rtn_clause;

// 	for(auto val: this->ssa0_list)
// 	{
// 		rtn_clause.ssa0_list.push_back(val); 
// 	}
// 	for(auto val: this->ssa1_list)
// 	{
// 		rtn_clause.ssa1_list.push_back(val); 
// 	}
// 	return rtn_clause;
// }

DFS_clause DFS_clause::operator+(DFS_clause src_clause)
{
    DFS_clause rtn_clause;

	std::map<int,int> sum_map_0, sum_map_1;
	for(auto val: this->ssa0_list)
	{
		sum_map_0[val]++; 
	}
	for(auto val: src_clause.ssa0_list)
	{
		sum_map_0[val]++; 
	}
	for(auto iter: sum_map_0)
	{
		if(iter.second > 0)
		{
			rtn_clause.ssa0_list.push_back(iter.first);
		}
	}

    for(auto val: this->ssa1_list)
	{
		sum_map_1[val]++; 
	}
	for(auto val: src_clause.ssa1_list)
	{
		sum_map_1[val]++; 
	}
	for(auto iter: sum_map_1)
	{
		if(iter.second > 0)
		{
			rtn_clause.ssa1_list.push_back(iter.first);
		}
	}


	return rtn_clause;

}


DFS_clause DFS_clause::operator-(DFS_clause src_clause)
{
    DFS_clause rtn_clause;

	std::map<int,int> sum_map_0, sum_map_1;
	for(auto val: this->ssa0_list)
	{
		sum_map_0[val]++; 
	}
	for(auto val: src_clause.ssa0_list)
	{
		sum_map_0[val]--; 
	}
	for(auto iter: sum_map_0)
	{
		if(iter.second == 1)
		{
			rtn_clause.ssa0_list.push_back(iter.first);
		}
	}

    for(auto val: this->ssa1_list)
	{
		sum_map_1[val]++; 
	}
	for(auto val: src_clause.ssa1_list)
	{
		sum_map_1[val]--; 
	}
	for(auto iter: sum_map_1)
	{
		if(iter.second == 1)
		{
			rtn_clause.ssa1_list.push_back(iter.first);
		}
	}
	return rtn_clause;
}


DFS_clause DFS_clause::operator&(DFS_clause src_clause)
{
    DFS_clause rtn_clause;

	std::map<int,int> sum_map_0, sum_map_1;
	for(auto val: this->ssa0_list)
	{
		sum_map_0[val]++; 
	}
	for(auto val: src_clause.ssa0_list)
	{
		sum_map_0[val]++; 
	}
	for(auto iter: sum_map_0)
	{
		if(iter.second == 2)
		{
			rtn_clause.ssa0_list.push_back(iter.first);
		}
	}

    for(auto val: this->ssa1_list)
	{
		sum_map_1[val]++; 
	}
	for(auto val: src_clause.ssa1_list)
	{
		sum_map_1[val]++; 
	}
	for(auto iter: sum_map_1)
	{
		if(iter.second == 2)
		{
			rtn_clause.ssa1_list.push_back(iter.first);
		}
	}
	return rtn_clause;
}


void DFS_clause::add_myself(int node_id, int node_val)
{
    if(node_val == 0)
    {
        this->ssa0_list.push_back(node_id);
        // std::sort(ssa0_list.begin(), ssa0_list.end())
    }
    else if(node_val == 1)
    {
        this->ssa1_list.push_back(node_id);
    }
    else
    {
        std::cout<<"node id "<<node_id<<" has an illegal value "<<node_val<<std::endl;
    }
    
}

void DFS_clause::print_DFS(void)
{
	std::cout<<"ssa0: ";
	for(auto elem: this->ssa0_list)
	{
		std::cout<<elem<<"  ";
	}
	std::cout<<std::endl<<"ssa1: ";
	for(auto elem: this->ssa1_list)
	{
		std::cout<<elem<<"  ";
	}
	std::cout<<std::endl;
}

void circuit::DFS(void)
{
    int control_exist;
	node* fin_gate;


    if(this->simulation_flag == 0)
    {
        std::cout<<"simulation should be done before DFS!"<<std::endl;
        return;
    }

	// print out table content
	// for(int i = 0; i<=max_lvl; i++){
    //     cout<<"level = "<<i;
    //     for(auto x : table[i]){
    //         cout<<"-->"<<x->num;
    //     }
    //     cout<<endl;
    // }
    //std::cout<<"get into DFS!"<<std::endl;
    for (int i = 0; i <= this->max_lvl; i++)
    { // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
		
        for(auto x : this->table[i])
        {
			DFS_clause control_result;
			DFS_clause non_control_result;
            if(x->type == e_gtype::IPT)
            {	
                x->ssa_clause.add_myself(x->num, !x->val);
            }
            else if(x->type == e_gtype::BRCH || x->type == e_gtype::NOT)
            {
                x->ssa_clause = x->unodes[0]->ssa_clause;
                x->ssa_clause.add_myself(x->num, !x->val);
            }
            else
            {
                control_exist = 0;
                // for(node* fin_gate: x->unodes)
				// for(auto fin_gate: x->unodes)
				for(int j = 0; j<x->fin; j++)
                {
					// std::cout << "debug unodes";
					// x->pn();
					fin_gate = x->unodes[j];
                    if((x->type == e_gtype::AND || x->type == e_gtype::NAND) && (fin_gate->val == 0))	// controlling value for AND and NAND gate
                    {
						if(control_exist == 0)
						{
							control_result = fin_gate->ssa_clause;
						}
						else
						{
							control_result = control_result & fin_gate->ssa_clause;
						}
                        control_exist = 1;
                    }
					else if((x->type == e_gtype::AND || x->type == e_gtype::NAND) && (fin_gate->val == 1)) // non-controlling value for AND and NAND gate
					{
						non_control_result = non_control_result + fin_gate->ssa_clause;
					}
                    else if((x->type == e_gtype::OR || x->type == e_gtype::NOR) && (fin_gate->val == 1))	// controlling value for OR and NOR gate
                    {
						if(control_exist == 0)
						{
							control_result = fin_gate->ssa_clause;
						}
						else
						{
							control_result = control_result & fin_gate->ssa_clause;
						}
                        control_exist = 1;
                    }
					else if((x->type == e_gtype::OR || x->type == e_gtype::NOR) && (fin_gate->val == 0)) // non-controlling value for OR and NOR gate
					{
						non_control_result = non_control_result + fin_gate->ssa_clause;
					}
					else if(x->type == e_gtype::XOR)
					{
						non_control_result = non_control_result + fin_gate->ssa_clause;
					}
					else
					{
						std::cout<<"meet a wrong situation! in function void circuit::DFS(void);"<<std::endl;
					}
					
                } // end for(auto fin_gate: x->unodes)

				if(control_exist == 1)
				{
					x->ssa_clause = control_result - non_control_result;
					x->ssa_clause.add_myself(x->num, !x->val);
				}
				else
				{
					x->ssa_clause = non_control_result;
					x->ssa_clause.add_myself(x->num, !x->val);
				}
            }
        }

    }
	// for(node* n: Node){
	// 	if(n->num == 6){
			
	// 	}
	// }
//	std::cout<<"---------------- DFS finished ----------------"<<std::endl;

}


void circuit::DFS_continuous(string input_file_name, string output_file_name)
{
    char logic_bit;
    std::vector<std::vector<char>> TP_vector = this->log_read_TP(input_file_name);
    DFS_clause overall_fault;
    for (vector<char> TP: TP_vector)
    {
        vector<char> log_sim_5_res_row; // result vector for store one TP
        for(node* PI_node: Pinput)
        {
            logic_bit = TP[PI_node->TP_idx];
            if(logic_bit == '1')
            {
                PI_node->val = 1;
            }
            else if(logic_bit == '0')
            {
                PI_node->val = 0;
            }
        }
        this->log_sim();
        this->DFS();
        for(node *PO_node: Poutput)
        {
            overall_fault = overall_fault + PO_node->ssa_clause;
        }
    }

    fp.open(output_file_name, ios::out);

    for(int node_id: overall_fault.ssa0_list)
    {  
        fp<<node_id<<"@0"<<endl;
    }
    for(int node_id: overall_fault.ssa1_list)
    {
        fp<<node_id<<"@1"<<endl;
    }

    fp.close();

}


DFS_clause circuit::DFS_ATPG()
{
    char logic_bit;
    DFS_clause overall_fault;
    for(node* PI_node: Pinput){
        // logic_bit = PI_node->logic_5_to_char(PI_node->PODEM_val_temp);
		if(PI_node->PODEM_val_temp == LOGIC_1 || PI_node->PODEM_val_temp == LOGIC_D)
			PI_node->val = 1;
		else
		{
			PI_node->val = 0;
		}
        // switch (logic_bit){
        //     case '1': PI_node->val = 1; break;
		// 	case 'D': PI_node->val = 1; break;
        //     case '0': PI_node->val = 0; break;
        //     default: PI_node->val = 0; break;
        // }
    }
    this->log_sim();
    this->DFS();
    for(node *PO_node: Poutput){
        overall_fault = overall_fault + PO_node->ssa_clause;
    }

    // for(int node_id: overall_fault.ssa0_list) fp<<node_id<<"@0"<<endl;
    // for(int node_id: overall_fault.ssa1_list) fp<<node_id<<"@1"<<endl;

    return overall_fault;
}

DFS_clause circuit::DFS_ATPG_DALG(){
	const int DALG_rst_size = DALG_rst_num.size();
    uint8_t logic_bit;
    DFS_clause overall_fault;
    // for(node* PI_node: Pinput){
	// 	for(int i=0; i<DALG_rst_size;i++)
	// 	{
	// 		if(PI_node->num == DALG_rst_num[i]){
	// 			logic_bit = PI_node->logic_5_to_char(DALG_rst_val5[i]);
	// 			break;
	// 		}
	// 	}
        
    //     switch (logic_bit){
    //         case '1': PI_node->val = 1; break;
	// 		case 'D': PI_node->val = 1; break;
    //         case '0': PI_node->val = 0; break;
    //         default: PI_node->val = 0; break;
    //     }
    // }
	for(int i = 0; i < DALG_rst_size; i++)
	{
		logic_bit = DALG_rst_val5[i];
		if(logic_bit == LOGIC_1 || logic_bit == LOGIC_D)
			this->Node[this->num_to_idx[DALG_rst_num[i]]]->val = 1;
		else
		{
			this->Node[this->num_to_idx[DALG_rst_num[i]]]->val = 0;
		}
		
	}
    this->log_sim();
    this->DFS();
    for(node *PO_node: Poutput){
        overall_fault = overall_fault + PO_node->ssa_clause;
    }

    // for(int node_id: overall_fault.ssa0_list) fp<<node_id<<"@0"<<endl;
    // for(int node_id: overall_fault.ssa1_list) fp<<node_id<<"@1"<<endl;

    return overall_fault;
}
