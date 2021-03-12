#include "circuit.hpp"
#include "PODEM.hpp"
#include <list>
#include <vector>
#include <iostream>

// #define PODEM_PRINT_D_FROMTIER 1
// #define PODEM_PRINT_J_FROMTIER 1

#define PODEM_BACKTRACE_NON_DFS 1

#define ONLY_FWD_IMPLY 1


InC_result circuit::PODEM_fwd_imply(void)
{
    uint8_t temp;
    for (int j = 1; j <= this->max_lvl; j++)
    { // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
        for(node* curr_gate: this->Node)
        {
            if (curr_gate->lvl == j)
            {
                switch(curr_gate->type)
                {
                        
                    case e_gtype::BRCH:
                        curr_gate->PODEM_val_temp = curr_gate->unodes[0]->PODEM_val_temp;
                        //cout<<"it's branck"<<endl;
                        break;
                    
                    case e_gtype::XOR:
                        temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            if(fan_in_node->PODEM_val_temp == LOGIC_X || fan_in_node->PODEM_val_temp == LOGIC_XX)
                            {
                                temp = LOGIC_X;
                                break;
                            }
                            temp = temp ^ fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                        {
                            curr_gate->PODEM_val_temp = temp;
                        }
                        else
                        {
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        }
                        break;
                        
                    case  e_gtype::OR:
                        temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp | fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                        {
                            curr_gate->PODEM_val_temp = temp;
                        }
                        else
                        {
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        }
                        break;
                        
                    case  e_gtype::NOR:
                        temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp | fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = ~temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        break;
                    
                    case  e_gtype::NOT:
                        curr_gate->PODEM_val_temp = ~curr_gate->unodes[0]->PODEM_val_temp;
                        if(curr_gate->PODEM_val_temp != LOGIC_0 && curr_gate->PODEM_val_temp != LOGIC_1 && curr_gate->PODEM_val_temp != LOGIC_D && curr_gate->PODEM_val_temp != LOGIC_Dn)
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        break;
       
                    case e_gtype::NAND:
                        temp = 0xFF;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp & fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = ~temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        break;

                    
                    case  e_gtype::AND:
                        temp = 0xFF;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp & fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        break;
                    
                    default: 
                        std::cout<<"it's not valid type !"<<std::endl;
                        break;
                     
                } // swith type
                if(curr_gate->PODEM_val_temp == LOGIC_0 && curr_gate->num == this->PODEM_ssa_node && this->PODEM_ssa_val == 1)
                    curr_gate->PODEM_val_temp = LOGIC_Dn;
                else if (curr_gate->PODEM_val_temp == LOGIC_1 && curr_gate->num == this->PODEM_ssa_node && this->PODEM_ssa_val == 0)
                    curr_gate->PODEM_val_temp = LOGIC_D;
            } // if lvl = j
            
        } // for i < Nnodes
    }// for j < max_lvl

    //cout << "size of log_sim_5 vector is: " << log_sim_5_result.size() << endl;
    //cout << endl;

    return InC_result::InC_SUCCESS;
}

InC_result circuit::PODEM_imply(void)
{
    node* curr_gate;
    int update_in_flag, update_out_flag, update_branch_flag;

    std::vector<node*> in_0_vec, in_1_vec, in_x_vec, in_d_vec, in_dn_vec;
    while(this->PODEM_Imply_vector.size()!=0)
    {
        update_in_flag = 0;
        update_out_flag = 0;
        update_branch_flag = 0;
        curr_gate = this->PODEM_Imply_vector[0];
        this->PODEM_Imply_vector.erase(this->PODEM_Imply_vector.begin());
        if(this->PODEM_Imply_map[curr_gate->num])
            this->PODEM_Imply_map.erase(curr_gate->num);

        if(curr_gate->type == e_gtype::IPT)
            continue;

        in_0_vec.clear();
        in_1_vec.clear();
        in_d_vec.clear();
        in_dn_vec.clear();
        in_x_vec.clear();

        for(auto fin_node: curr_gate->unodes)
        {
            switch(fin_node->PODEM_val_temp)
            {
                case LOGIC_0:
                    in_0_vec.push_back(fin_node);
                    break;
                case LOGIC_1:
                    in_1_vec.push_back(fin_node);
                    break;
                case LOGIC_D:
                    in_d_vec.push_back(fin_node);
                    break;
                case LOGIC_Dn:
                    in_dn_vec.push_back(fin_node);
                    break;
                default:    // case LOGIC_X
                    in_x_vec.push_back(fin_node);
                    break;
            }
        }
#ifndef ONLY_FWD_IMPLY
        if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX) // output value is defined, from output to input
        {
            switch(curr_gate->type)
            {
                case e_gtype::BRCH:
                    if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                        curr_gate->unodes[0]->PODEM_val_temp = LOGIC_0;
                    else if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                        curr_gate->unodes[0]->PODEM_val_temp = LOGIC_1;
                    for(node* BRA_neighbor:curr_gate->unodes[0]->dnodes)    // for other branch of the same stem
                    {
                        if(BRA_neighbor == curr_gate)   // if this the dnode is myself
                        {
                            continue;
                        }
                        BRA_neighbor->PODEM_val_temp = curr_gate->PODEM_val_temp;
                        if(this->PODEM_Imply_map[BRA_neighbor->dnodes[0]->num]==0)
                        {
                            this->PODEM_Imply_map[BRA_neighbor->dnodes[0]->num] = 1;
                            this->PODEM_Imply_vector.push_back(BRA_neighbor->dnodes[0]);
                        }
                    }
                    update_in_flag = 1;
                    update_branch_flag = 1;
                    break;

                case e_gtype::NOT:
                    if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                        curr_gate->unodes[0]->PODEM_val_temp = LOGIC_1;
                    else if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                        curr_gate->unodes[0]->PODEM_val_temp = LOGIC_0;
                    update_in_flag = 1;
                    break;

                case e_gtype::AND:
                    if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                    {
                        for(node* unknown_gate:in_x_vec)
                        {
                            unknown_gate->PODEM_val_temp = LOGIC_1;
                        }
                        if(in_x_vec.size()!=0)
                        {
                            update_in_flag = 1;
                        }
                    }
                    else if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                    {
                        if(in_x_vec.size() == 1 && in_0_vec.size() == 0)    // only if OP needs 0, there is no 0 in IP, and only 1 known val
                        {
                            in_0_vec[0]->PODEM_val_temp = LOGIC_0;
                            update_in_flag = 1;
                        }
                    }
                    break;

                case e_gtype::NAND:
                    if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                    {
                        for(node* unknown_gate:in_x_vec)
                        {
                            unknown_gate->PODEM_val_temp = LOGIC_1;
                        }
                        if(in_x_vec.size()!=0)
                        {
                            update_in_flag = 1;
                        }
                    }
                    else if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                    {
                        if(in_x_vec.size() == 1 && in_0_vec.size() == 0)    // only if OP needs 0, there is no 0 in IP, and only 1 known val
                        {
                            in_0_vec[0]->PODEM_val_temp = LOGIC_0;
                            update_in_flag = 1;
                        }
                    }
                    break;

                case e_gtype::OR:
                    if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                    {
                        for(node* unknown_gate:in_x_vec)
                        {
                            unknown_gate->PODEM_val_temp = LOGIC_0;
                        }
                        if(in_x_vec.size()!=0)
                        {
                            update_in_flag = 1;
                        }
                    }
                    else if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                    {
                        if(in_x_vec.size() == 1 && in_1_vec.size() == 0)    // only if OP needs 0, there is no 0 in IP, and only 1 known val
                        {
                            in_1_vec[0]->PODEM_val_temp = LOGIC_1;
                            update_in_flag = 1;
                        }
                    }
                    break;

                case e_gtype::NOR:
                    if(curr_gate->PODEM_val_temp == LOGIC_1 || curr_gate->PODEM_val_temp == LOGIC_D)
                    {
                        for(node* unknown_gate:in_x_vec)
                        {
                            unknown_gate->PODEM_val_temp = LOGIC_0;
                        }
                        if(in_x_vec.size()!=0)
                        {
                            update_in_flag = 1;
                        }
                    }
                    else if(curr_gate->PODEM_val_temp == LOGIC_0 || curr_gate->PODEM_val_temp == LOGIC_Dn)
                    {
                        if(in_x_vec.size() == 1 && in_1_vec.size() == 0)    // only if OP needs 0, there is no 0 in IP, and only 1 known val
                        {
                            in_1_vec[0]->PODEM_val_temp = LOGIC_1;
                            update_in_flag = 1;
                        }
                    }
                    break;

                case e_gtype::XOR:
                    if(in_x_vec.size()==1)
                    {
                        uint8_t temp = curr_gate->PODEM_val_temp;
                        for(node* fan_in_gate:curr_gate->unodes)
                        {
                            temp = temp ^ fan_in_gate->PODEM_val_temp;
                        }
                        in_x_vec[0]->PODEM_val_temp = temp;
                        update_in_flag = 1;
                    }
                    break;
            }   // end switch(curr_gate->type)
        }   // end if(curr_gate->PODEM_val_temp != logic_5_typedef::LOGIC_X)
        else    // from input to output
#else
        if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
#endif
        {
            switch(curr_gate->type)
            {
                case e_gtype::BRCH:
                    curr_gate->PODEM_val_temp = curr_gate->unodes[0]->PODEM_val_temp;
                    update_out_flag = 1;
                    break;

                case e_gtype::NOT:
                    curr_gate->PODEM_val_temp = ~curr_gate->unodes[0]->PODEM_val_temp;
                    if(curr_gate->PODEM_val_temp != LOGIC_X || curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;

                case e_gtype::AND:

                    if(in_0_vec.size() != 0)
                    {
                        curr_gate->PODEM_val_temp = LOGIC_0;
                    }
                    else
                    {
                        uint8_t temp = 0xFF;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp & fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                    }
                    if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;

                case e_gtype::NAND:
                    if(in_0_vec.size() != 0)
                    {
                        curr_gate->PODEM_val_temp = LOGIC_0;
                    }
                    else
                    {
                        uint8_t temp = 0xFF;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp & fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = ~temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                    }
                    if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;

                case e_gtype::OR:
                    if(in_0_vec.size() != 0)
                    {
                        curr_gate->PODEM_val_temp = LOGIC_0;
                    }
                    else
                    {
                        uint8_t temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp | fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                    }
                    if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;

                case e_gtype::NOR:
                    if(in_0_vec.size() != 0)
                    {
                        curr_gate->PODEM_val_temp = LOGIC_0;
                    }
                    else
                    {
                        uint8_t temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp | fan_in_node->PODEM_val_temp;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = ~temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                    }
                    if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;

                case e_gtype::XOR:
                    if(in_x_vec.size() != 0)
                        curr_gate->PODEM_val_temp = LOGIC_X;
                    else
                    {
                        uint8_t temp = 0;
                        for(node* fan_in_node:curr_gate->unodes)
                        {
                            temp = temp ^ fan_in_node->value_5;
                        }
                        if(temp == LOGIC_0 || temp == LOGIC_1 || temp == LOGIC_D || temp == LOGIC_Dn)
                            curr_gate->PODEM_val_temp = temp;
                        else
                            curr_gate->PODEM_val_temp = LOGIC_X;
                        break;
                    }
                    if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
                        update_out_flag = 1;
                    break;
            }   // end switch(curr_gate->type)
        }

        if(curr_gate->num == this->PODEM_ssa_node)
        {
            if(this->PODEM_ssa_val == 1 && curr_gate->PODEM_val_temp == LOGIC_1)
            {
                curr_gate->PODEM_val_temp == LOGIC_D;
            }
            else if(this->PODEM_ssa_val == 0 && curr_gate->PODEM_val_temp == LOGIC_0)
            {
                curr_gate->PODEM_val_temp == LOGIC_Dn;
            }
        }

        if(update_in_flag)
        {
            for(node* unknown_gate: in_x_vec)
            {
                if(this->PODEM_Imply_map[unknown_gate->num]==0 && unknown_gate->PODEM_val_temp != LOGIC_X && unknown_gate->PODEM_val_temp != LOGIC_XX)
                {
                    this->PODEM_Imply_map[unknown_gate->num] = 1;
                    this->PODEM_Imply_vector.push_back(unknown_gate);
                }
            }
        }

        if(update_out_flag)
        {
            for(node* fan_out_gate: curr_gate->dnodes)
            {
                if(this->PODEM_Imply_map[fan_out_gate->num]==0)
                {
                    this->PODEM_Imply_map[fan_out_gate->num] = 1;
                    this->PODEM_Imply_vector.push_back(fan_out_gate);
                }
            }
        }
    }

    return InC_result::InC_SUCCESS;
}

InC_result circuit::PODEM_imply(std::map<int, uint8_t> known_set)
// E.g. if we want to imply A = 1, B = 1 and C = 0, location = <A,B,C>, value = <1,1,0>
{
    InC_result rtn_result;

    this->PODEM_Imply_map.clear();
    this->PODEM_Imply_vector.clear();

    for(node* each_gate: this->Node)
    {
        each_gate->PODEM_val_temp = LOGIC_X;
    }

    for(auto value_pair: known_set)
    {
        if(this->Node[this->num_to_idx[value_pair.first]]->type == e_gtype::IPT)
        {
            this->Node[this->num_to_idx[value_pair.first]]->PODEM_val_temp = value_pair.second;
        }
    }

    rtn_result = this->PODEM_fwd_imply();

    this->PODEM_get_D_Frontier(this->PODEM_ssa_node);

    return rtn_result;

}





int circuit::PODEM_error_at_PO(void)
{
    for(node* PO_to_check: this->Poutput)
    {
        if(PO_to_check->PODEM_val_temp == LOGIC_D || PO_to_check->PODEM_val_temp == LOGIC_Dn)
        {
            return 1;
        }
    }
    return 0;
}




void circuit::PODEM_get_D_Frontier(int fault_location)
{
    node* fault_src = this->Node[this->num_to_idx[fault_location]];
    // node* curr_gate;

    std::vector<node*> pending_list;

    bool in_d_dn_cnt;

    this->PODEM_D_Frontier.clear();
    this->PODEM_D_Frontier_map.clear();

//    this->dump_all_nodes();


    for(node* curr_gate: this->Node)
    {
#ifdef PODEM_PRINT_D_FROMTIER
        // std::cout<<"inside tranversing, current gate is "<<curr_gate->num<<std::endl;
#endif
        if(curr_gate->type == e_gtype::IPT||curr_gate->type == e_gtype::NOT||curr_gate->type == e_gtype::BRCH)
        {
            // std::cout<<"skip node because of IPT "<<curr_gate->num<<std::endl;
            continue;
        }
        if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
        {
            // std::cout<<"skip node because of XXX "<<curr_gate->num<<std::endl;
            continue;
        }
        in_d_dn_cnt = false;
        // std::cout<<"current node: "<<curr_gate->num<<std::endl;
        for(node* fan_in_gate: curr_gate->unodes)
        {
            // std::cout<<"fin node: "<<fan_in_gate->num<<std::endl;
            if(fan_in_gate->PODEM_val_temp == LOGIC_D || fan_in_gate->PODEM_val_temp == LOGIC_Dn)
            {
                // std::cout<<"adding node "<<curr_gate->num<<" to D-F"<<std::endl;
                in_d_dn_cnt = true;
                break;
            }
        }
        if(in_d_dn_cnt)
        {
            this->PODEM_D_Frontier.push_back(curr_gate);
            this->PODEM_D_Frontier_map[curr_gate->num] = 1;
        }
    }

    // this->dump_all_nodes();
    // for(node* fan_out_node: fault_src->dnodes)
    //     pending_list.push_back(fan_out_node);

    // while(pending_list.size() != 0)
    // {
    //     curr_gate = pending_list[0];
    //     pending_list.erase(pending_list.begin());

    //     if(curr_gate->PODEM_val_temp != LOGIC_X && curr_gate->PODEM_val_temp != LOGIC_XX)
    //     {
    //         continue;
    //     }
    //     in_d_dn_cnt = 0;
    //     for(node* fan_in_gate: curr_gate->unodes)
    //     {
    //         if(fan_in_gate->PODEM_val_temp == LOGIC_D || fan_in_gate->PODEM_val_temp == LOGIC_Dn)
    //         {
    //             in_d_dn_cnt++;
    //             break;
    //         }
    //     }
    //     if(in_d_dn_cnt)
    //     {
    //         if(this->PODEM_D_Frontier_map[curr_gate->num] == 0)
    //         {
    //             this->PODEM_D_Frontier.push_back(curr_gate);
    //             this->PODEM_D_Frontier_map[curr_gate->num] = 1;
    //             for(node* fan_out_gate: curr_gate->dnodes)
    //             {
    //                 pending_list.push_back(fan_out_gate);
    //             }
    //         }
    //     }
    // }

#ifdef PODEM_PRINT_D_FROMTIER
    std::cout<<"start print DF in PODEM: ";
    for(node* DF_in_list: this->PODEM_D_Frontier)
    {
        std::cout<< DF_in_list->num << "   ";
    }
    std::cout<<std::endl;
#endif

}


void circuit::PODEM_get_J_Frontier(int fault_location)
{
    node* fault_src = this->Node[this->num_to_idx[fault_location]];
    node* curr_gate;

    std::vector<node*> pending_list;

    int in_x_cnt;

    this->PODEM_J_Frontier.clear();
    this->PODEM_J_Frontier_map.clear();

    pending_list.push_back(fault_src);

    while(pending_list.size() != 0)
    {
        curr_gate = pending_list[0];
        pending_list.erase(pending_list.begin());

        if(curr_gate->PODEM_val_temp == LOGIC_X || curr_gate->PODEM_val_temp == LOGIC_XX)
        {
            continue;
        }
        in_x_cnt = 0;
        for(node* fan_in_gate: curr_gate->unodes)
        {
            if(fan_in_gate->PODEM_val_temp == LOGIC_X || curr_gate->PODEM_val_temp == LOGIC_XX)
            {
                in_x_cnt++;
                break;
            }
        }
        if(in_x_cnt)
        {
            if(this->PODEM_J_Frontier_map[curr_gate->num] == 0)
            {
                this->PODEM_J_Frontier.push_back(curr_gate);
                this->PODEM_J_Frontier_map[curr_gate->num] = 1;
                for(node* fan_in_gate: curr_gate->unodes)
                {
                    pending_list.push_back(fan_in_gate);
                }
            }
        }
    }

#ifdef PODEM_PRINT_J_FROMTIER
    std::cout<<"start print JF in PODEM: ";
    for(node* JF_in_list: this->PODEM_J_Frontier)
    {
        std::cout<< JF_in_list->num << "   ";
    }
    std::cout<<std::endl;
#endif

}


std::pair<int, uint8_t> circuit::PODEM_backtrace_non_DFS(std::pair<int, uint8_t> obj_result)
{
    // std::cout<<"enter PODEM_backtrace_non_DFS"<<std::endl;
    node* target_node = this->Node[this->num_to_idx[obj_result.first]];

    std::vector<node*> backtrace_vec;

    node* curr_gate = target_node;

    std::pair<int, uint8_t> rtn_pair;
    int inversion_val = 0;

    int PI_found = 0;

    int fan_in_flag_cnt = 0;
/*
    while(curr_gate->type != e_gtype::IPT)
    {
        for(node* fan_in_node: curr_gate->unodes)
        {
            if((fan_in_node->PODEM_val_fixed == LOGIC_X || fan_in_node->PODEM_val_fixed == LOGIC_XX) && fan_in_node->PODEM_backtrace_check_flag == 0)
//            if(fan_in_node->PODEM_val_fixed == LOGIC_X || fan_in_node->PODEM_val_fixed == LOGIC_XX)
            {
                if(curr_gate->type == e_gtype::NAND || curr_gate->type == e_gtype::NOR || curr_gate->type == e_gtype::NOT)
                {
                    inversion_val++;
                }
                curr_gate = fan_in_node;
                if(curr_gate->type == e_gtype::IPT)
                {
                    PI_found = 1;
                }
                break;
            }
        }
    }
    std::cout<<"PI found is "<< curr_gate->num<<std::endl;
    std::cout<<"back trace is done, starting setting flag!"<<std::endl;
    if(PI_found)
    {
        curr_gate->PODEM_backtrace_check_flag = 1;
        rtn_pair.first = curr_gate->num;
        for(node* fan_out_node: curr_gate->dnodes)
        {
            backtrace_vec.push_back(fan_out_node);
        }
        while(backtrace_vec.size() != 0)
        {
            curr_gate = backtrace_vec[0];
            backtrace_vec.erase(backtrace_vec.begin());
            fan_in_flag_cnt = 0;
            for(node* fan_in_node: curr_gate->unodes)
            {
                if(fan_in_node->PODEM_backtrace_check_flag == 0)
                {
                    break;
                }
                else
                {
                    fan_in_flag_cnt++;
                }
            }
            if(fan_in_flag_cnt == curr_gate->unodes.size())
            {
                curr_gate->PODEM_backtrace_check_flag = 1;
                for(node* fan_out_node: curr_gate->dnodes)
                {
                    backtrace_vec.push_back(fan_out_node);
                }
            }
        }
        inversion_val = inversion_val % 2;
        if(inversion_val)
        {
            if(obj_result.second == LOGIC_0 || obj_result.second == LOGIC_Dn)
            {
                rtn_pair.second = LOGIC_1;
            }
            else
            {
                rtn_pair.second = LOGIC_0;
            }
        }
    }
    else
    {
        rtn_pair.first = -1;
        rtn_pair.second = LOGIC_X;
    }

    // delete target_node;
    // delete curr_gate;
    return rtn_pair;
*/
    while(curr_gate->type != e_gtype::IPT)
    {
        for(node* fan_in_node: curr_gate->unodes)
        {
            // if((fan_in_node->PODEM_val_fixed == LOGIC_X || fan_in_node->PODEM_val_fixed == LOGIC_XX) && fan_in_node->PODEM_backtrace_check_flag == 0)
           if(fan_in_node->PODEM_val_temp == LOGIC_X || fan_in_node->PODEM_val_temp == LOGIC_XX)
            {
                if(curr_gate->type == e_gtype::NAND || curr_gate->type == e_gtype::NOR || curr_gate->type == e_gtype::NOT)
                {
                    inversion_val++;
                }
                curr_gate = fan_in_node;
                if(curr_gate->type == e_gtype::IPT)
                {
                    PI_found = 1;
                }
                break;
            }
        }
    }

    if(PI_found)
    {
        rtn_pair.first = curr_gate->num;
        inversion_val = inversion_val % 2;
        if(inversion_val)
        {
            if(obj_result.second == LOGIC_0 || obj_result.second == LOGIC_Dn)
            {
                rtn_pair.second = LOGIC_1;
            }
            else
            {
                rtn_pair.second = LOGIC_0;
            }
        }
    }
    else
    {
        rtn_pair.first = -1;
        rtn_pair.second = LOGIC_X;
    }

    // delete target_node;
    // delete curr_gate;
    return rtn_pair;
}

std::pair<int, uint8_t> circuit::PODEM_objective(void)
{
    std::pair <int, uint8_t> rtn_pair;
    if(this->Node[this->num_to_idx[this->PODEM_ssa_node]]->PODEM_val_temp == LOGIC_X || this->Node[this->num_to_idx[this->PODEM_ssa_node]]->PODEM_val_temp == LOGIC_XX)
    {
        if(this->PODEM_ssa_val == 1)
        {
            rtn_pair = std::make_pair(this->PODEM_ssa_node, LOGIC_Dn);
        }
        else
        {
            rtn_pair = std::make_pair(this->PODEM_ssa_node, LOGIC_D);
        }
        return rtn_pair;
    }

    node* target_node = this->PODEM_D_Frontier[0];
    for(node* fan_in_node: target_node->unodes)
    {
        if(fan_in_node->PODEM_val_temp == LOGIC_X || fan_in_node->PODEM_val_temp == LOGIC_XX)
        {
            if(target_node->type == e_gtype::AND || target_node->type == e_gtype::NAND)
            {
                rtn_pair = std::make_pair(fan_in_node->num, LOGIC_1);
            }
            else
            {
                rtn_pair = std::make_pair(fan_in_node->num, LOGIC_0);
            }
        }
    }
    return rtn_pair;
}


void circuit::PODEM_set_fixed_val(std::pair<int, uint8_t> obj_pair)
{
    if(obj_pair.second == LOGIC_X || obj_pair.second == LOGIC_XX)
    {
        auto find_result = this->PODEM_fixed_value_map.find(obj_pair.first);
        if(find_result != this->PODEM_fixed_value_map.end())
        {
            this->PODEM_fixed_value_map.erase(find_result);
        }
    }
    else
    {
        this->PODEM_fixed_value_map[obj_pair.first] = obj_pair.second;
    }
}



PODEM_result circuit::PODEM(int PODEM_initial, int &iter_num)
{
    if(iter_num > this->PODEM_max_iter)
    {
        return PODEM_result::PODEM_FAIL;
    }
    if(PODEM_initial == 0)
    {
        if(this->PODEM_error_at_PO() == 1)
        {
            // std::cout<<"PODEM success"<<std::endl;
            return PODEM_result::PODEM_SUCCESS;
        }
        if(this->PODEM_D_Frontier.size() == 0 && (this->Node[this->num_to_idx[this->PODEM_ssa_node]]->PODEM_val_temp != LOGIC_X && this->Node[this->num_to_idx[this->PODEM_ssa_node]]->PODEM_val_temp != LOGIC_XX))
        {
            // std::cout<<"PODEM fail"<<std::endl;
            return PODEM_result::PODEM_FAIL;
        }
    }
    else
    {
        for (int i = 0; i < Nnodes; i++){  // initialize the value of circuit in PODEM backtrace
            Node[i] -> PODEM_val_temp = LOGIC_X;
        }
#ifndef PODEM_BACKTRACE_NON_DFS
        PODEM_find_all_path();
#endif
    }

#ifndef PODEM_BACKTRACE_NON_DFS
    std::cout << "print all the path" << std::endl;
    for (auto & j : this->path_matrix) {
        for (int i = 0; i < j.size(); ++i) {
            std::cout << j[i] << "->";
        }
        std::cout << std::endl;
    }
    std::cout << "print all the path finished" << std::endl;
#endif
    std::pair<int, uint8_t> obj_pair, backtrace_pair;

    obj_pair = this->PODEM_objective();
    this->PODEM_set_fixed_val(obj_pair);

    // std::cout << "objective finished" << std::endl;
    // std::cout << "obj_pair.first: " << obj_pair.first << "; obj_pair.second: " << int(obj_pair.second) << std::endl; 

#ifdef PODEM_BACKTRACE_NON_DFS
    backtrace_pair = this->PODEM_backtrace_non_DFS(obj_pair);
#else
    backtrace_pair = this->PODEM_backtrace(obj_pair);
    //TODO: unify the val variable
#endif
    // std::cout << "backtrace finished" << std::endl;
    // std::cout << "backtrace_pair.first: " << backtrace_pair.first << "; backtrace_pair.second: " << int(backtrace_pair.second)<< std::endl; 

    this->PODEM_set_fixed_val(backtrace_pair);

    this->PODEM_imply(this->PODEM_fixed_value_map);

    iter_num++;

    if(this->PODEM(0, iter_num) == PODEM_result::PODEM_SUCCESS)
    {
        return PODEM_result::PODEM_SUCCESS;
    }

    backtrace_pair.second = ~backtrace_pair.second;

    this->PODEM_set_fixed_val(backtrace_pair);

    this->PODEM_imply(this->PODEM_fixed_value_map);

    iter_num++;

    if(this->PODEM(0, iter_num) == PODEM_result::PODEM_SUCCESS)
    {
        return PODEM_result::PODEM_SUCCESS;
    }

    backtrace_pair.second = LOGIC_X;

    this->PODEM_set_fixed_val(backtrace_pair);

    this->PODEM_imply(this->PODEM_fixed_value_map);

    return PODEM_result::PODEM_FAIL;


}




pair<int, uint8_t> circuit::PODEM_backtrace(std::pair<int, uint8_t> obj_result){

    np = this->Node[this->num_to_idx[obj_result.first]];  // find the fault node
    np ->PODEM_val_temp = obj_result.second;
    // find the PI node number and the inversion number to the fault
    pair<int, int> PI_and_inversions = PODEM_find_inversion_fault_to_PI(obj_result.first);
    uint8_t temp_val;
    if (obj_result.second == LOGIC_1 || obj_result.second == LOGIC_D){
        temp_val = LOGIC_1;
    }else{
        temp_val = LOGIC_0;
    }
    std::pair<int, uint8_t> rtn_pair;
    rtn_pair.first = PI_and_inversions.first; // assign the return pair first to the PI number
    if(PI_and_inversions.second % 2 == 0){  // assign the return pair second to the backtrace value
        rtn_pair.second = temp_val;
    } else{
        rtn_pair.second = ~temp_val;
    }
    return rtn_pair;
}

pair<int, int> circuit::PODEM_find_inversion_fault_to_PI(int fault_node_num) {
    for (int i = 0; i < path_matrix.size(); ++i) {  //path
        if(this->Node[this->num_to_idx[path_matrix[i][0]]]->PODEM_val_temp==LOGIC_X||
        this->Node[this->num_to_idx[path_matrix[i][0]]]->PODEM_val_temp==LOGIC_XX){  // if PI is  Logic X
            int num_inversions = 0;
            for (int j = 0; j < path_matrix[i].size(); ++j) {
                np = this->Node[this->num_to_idx[path_matrix[i][j]]]; // evaluate all node on path
                if (np->type == e_gtype::NOR || np->type == e_gtype::NOT || np->type == e_gtype::NAND){
                    num_inversions++;
                }
                if(np->num == fault_node_num){  // if fault node is on this path
                    pair<int, int> PI_inversion_pair = make_pair(path_matrix[i][0], num_inversions);
                    std::cout << "inversion: " << num_inversions << std::endl;
                    return PI_inversion_pair;
                }
            }
        }
    }
}

void circuit::PODEM_find_all_path() {
    bool* visited = new bool[Nnodes+1];
    vector<int> path;
    for (int i = 1; i < Nnodes+1; ++i) {     // Initialize all vertices as not visited
        visited[i] = false;
    }
    // Call the recursive helper function to print all paths
    for(node* PI_node: this->Pinput)
    {
        PODEM_PI_path(PI_node->num, visited, path);
    }
}

void circuit::PODEM_PI_path(int node_num, bool visited[], vector<int>& path) {
    // Mark the current node and store it in path[]
    node* local_node = this->Node[this->num_to_idx[node_num]];
    visited[node_num] = true;
    path.push_back(node_num);
    if(local_node->dnodes.empty()){  // end the recursion if the node is PO
        this->path_matrix.push_back(path);
        // for (int i = 0; i < path.size(); ++i) {
        //     cout << path[i] << " ";
        // }
        // cout << endl;
    } else{
        // Recur for all the vertices adjacent to current vertex
        for (int i = 0 ; i < local_node->dnodes.size(); i ++){
            if(!visited[local_node->dnodes[i]->num]){
                // cout << "denode_num: " << local_node->dnodes[i]->num << endl;
                PODEM_PI_path(local_node->dnodes[i]->num, visited, path);
            }
        }
    }
    path.pop_back();
    visited[node_num] = false; // Remove current vertex from path[] and mark it as unvisited
}


void circuit::dump_all_nodes()
{
    std::cout<<"--- start tranversing all nodes ---"<<std::endl;
    for(node* to_be_print: this->Node)
    {
        std::cout<<"node id "<<to_be_print->num<<"-";
        switch(to_be_print->PODEM_val_temp)
        {
            case LOGIC_0:
                std::cout<<"0"<<std::endl;
                break;
            case LOGIC_1:
                std::cout<<"1"<<std::endl;
                break;
            case LOGIC_D:
                std::cout<<"D"<<std::endl;
                break;
            case LOGIC_Dn:
                std::cout<<"DN"<<std::endl;
                break;
            case LOGIC_X:
            case LOGIC_XX:
                 std::cout<<"X"<<std::endl;
                break;
            default:
                std::cout<<"undefined value"<<int(to_be_print->PODEM_val_temp)<<std::endl;
                break;
        }
    }
    std::cout<<"---  end  tranversing all nodes ---"<<std::endl;
}

void circuit::PODEM_reset(void)
{
    this->PODEM_D_Frontier.clear();
    this->PODEM_J_Frontier.clear();
    this->PODEM_D_Frontier_map.clear();
    this->PODEM_J_Frontier_map.clear();

    this->PODEM_fixed_value_map.clear();
    this->PODEM_Imply_vector.clear();
    this->PODEM_Imply_map.clear();
}