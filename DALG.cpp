#include "circuit.hpp"

bool circuit:: DALG(int node_num, int fault, string ckt_fin, ofstream& dpf, int one_for_single_pattern_zero_for_multi_pattern){
    bool DALG_RESULT;
    
    DALG_reset();

    // Reset all node value in circuit and set the fault
    set_ckt_val5(node_num, fault);
    
    save_to_stk(); // before modifing the circuit, save current circuit
    cout << "FINDING " << node_num << " " << fault << endl;
    if(D_ALGORITHM() == false){
        cout << "=============================================" << endl;
        cout << "      DALG fail to find input for " << node_num << " " << fault << endl;
        cout << "=============================================" << endl;
        DALG_RESULT = false;
    }
    else{
        cout << "=============================================" << endl;
        cout << "       DALG find input for " << node_num << " " << fault << endl;
        cout << "=============================================" << endl;

        if(one_for_single_pattern_zero_for_multi_pattern == 1)
        {
            sDALG(node_num, fault, ckt_fin);
        }
        else if(one_for_single_pattern_zero_for_multi_pattern == 0)
        {
            sDALG_ATPG_pattern(dpf);
        }
        

        #ifdef DFS_REDUCTION_FOR_DALG
            DFS_clause faults_in_DFS_ATPG;
            faults_in_DFS_ATPG = DFS_ATPG_DALG();
            for(int node_id: faults_in_DFS_ATPG.ssa1_list){
    //            cout << node_id << "@1 add to the map by" << node << endl;
                this->DALG_coverage_map_1[node_id] = true;
            }
            for(int node_id: faults_in_DFS_ATPG.ssa0_list){
    //            cout << node_id << "@0 add to the map by" << node << endl;
                this->DALG_coverage_map_0[node_id] = true;
            }
        #endif

        DALG_RESULT = true;
    }

    string ckt_name = circuit::get_cname(ckt_fin);
    string TP_in = ckt_name + "_DALG_" + to_string(node_num) + "@" + to_string(fault) + ".txt";
    


    #ifdef DFS_CHECK_FOR_ATPG
    if(ATPG_rst_check(TP_in, "ATPG_check_tmp_file.txt",node_num,fault)){
        cout << "******************************************************" << endl;
        cout << "    DALG result has been checked by DFS -> CORRECT   "  << endl;
        cout << "******************************************************" << endl;
    }
    else{
        cout << "******************************************************" << endl;
        cout << "    DALG result has been checked by DFS -> WRONG   "  << endl;
        cout << "******************************************************" << endl;
    }

    remove("ATPG_check_tmp_file.txt");

    #endif

    return DALG_RESULT;
}

bool circuit:: D_ALGORITHM(){

    DALG_cnt++;

    if(DALG_cnt >= (this->Nnodes)*DALG_TO_coeff)
    {
        //cout << "--> DALG TIME OUT " << endl;
        return false;
    }

    if(ImpnC() == false){
        rec_from_stk(); // before return false, recovery circuit value
        return false;
    }
    
    if(err_at_PO() == false){
        if(D_fnter.empty()){
            rec_from_stk();
            return false;
        }

        while(!all_D_fnter_tried()){
            node* curr_D_fnter;
            for(pair<node*,int> iter: D_fnter){
                // select untried D_fnter
                if(iter.second==0){
                    curr_D_fnter = iter.first;
                    D_fnter[iter.first]++;
                    break;
                }
            }
            #ifdef DALG_DEBUG_INFO_ON
            std::cout<<"===================="<<std::endl<<"!!! D-F size is "<< this->D_fnter.size()<<" and J-F size is "<< this->J_fnter.size()<<std::endl;
            std::cout<<"===================="<< "!!! D-F content is " << std::endl;
            for(auto iter:D_fnter){
                cout << iter.first->num << " " << iter.second << endl; 
            }
            #endif
            
            save_to_stk(); // before modifing the circuit, save current circuit
            assign_cbar_to_D_fnter_fin(curr_D_fnter);

            if(D_ALGORITHM()==true){
                //rec_from_stk();
                return true;
            }
        }
        rec_from_stk();
        return false; // all D_fnter tried
    }
    else // error at PO
    {
        if(J_fnter.empty()){
            // TRUE case for D_ALGORITHM is done
            // Save Pinput to DLAG_rst
            save_DALG_rst(this->Pinput);
            #ifdef DALG_DEBUG_INFO_ON
            for(node* PI_node: this->Pinput)
            {
                std::cout<<"************ PI node "<<PI_node->num<<" has value "<<PI_node->logic_5_to_char(PI_node->value_5)<<std::endl;
            }
            #endif
            //rec_from_stk();
            //cout << "Test vector found in DALG" << endl;
            return true;
        }
        
        // select a gate from J_fnter
        node* curr_J_fnter;
        for(pair<node*,int> iter: J_fnter){
            if(iter.second==0) // this J_fnter is untried
            {
                curr_J_fnter = iter.first;
                J_fnter[iter.first]++;
                break;
            }
        }


        uint8_t c = LOGIC_X;
        uint8_t c_bar = LOGIC_X;
        get_c_cbar_for_curr_J_fnter(curr_J_fnter, c, c_bar);
        assert(c != LOGIC_X && c_bar != LOGIC_X);

        //save_to_stk(); // before modifing the circuit, save current circuit

        while(!all_inputs_of_curr_J_fnter_specified(curr_J_fnter)){
            node* sel_in; // selected input with value X from current J frontier
            for(node* unode: curr_J_fnter->unodes){
                if(unode->value_5==LOGIC_XX || unode->value_5==LOGIC_X){
                    sel_in = unode;
                    // save_to_stk(); // before modifing the circuit, save current circuit
                    UpdnC_val5(sel_in,c); // assign c to sel_in
                    break;
                }
            }
            save_to_stk(); // before modifing the circuit, save current circuit
            if(D_ALGORITHM()==true){
                //rec_from_stk();
                return true;
            }
            else // assign c_bar to sel_in
            {
                //rec_from_stk();
                sel_in->value_5 = LOGIC_X; // to avoid being treat as conflict in UpdnC_val5
                //save_to_stk(); // before modifing the circuit, save current circuit
                UpdnC_val5(sel_in,c_bar);
            }
        }
        rec_from_stk();
        return false;
    }
}

bool circuit:: ImpnC(){

    vector<node*> in_0_vec, in_1_vec, in_x_vec, in_d_vec, in_dn_vec;
    // int in_0_cnt, in_1_cnt, in_d_cnt, in_dn_cnt, in_x_cnt, in_total_cnt;

    while(!ImpnC_stk.empty())
    {

        node* np = ImpnC_stk.top();
        #ifdef DALG_DEBUG_INFO_ON
        std::cout<<"<<<< popping "<<np->num<<" from stack"<< std::endl;
        #endif
        ImpnC_stk.pop();

        in_0_vec.clear();
        in_1_vec.clear();
        in_d_vec.clear();
        in_dn_vec.clear();
        in_x_vec.clear();

        for(node* fin_node: np->unodes)
        {
            switch(fin_node->value_5)
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
                case LOGIC_X:
                case LOGIC_XX:
                    in_x_vec.push_back(fin_node);
                    break;
            }
        }

        const int in_0_cnt = in_0_vec.size();
        const int in_1_cnt = in_1_vec.size();
        const int in_d_cnt = in_d_vec.size();
        const int in_dn_cnt = in_dn_vec.size();
        const int in_x_cnt = in_x_vec.size();
        const int in_total_cnt = np->fin;

        uint8_t new_val5 = LOGIC_X, unode_val5 = LOGIC_X; // updated value through imply for np->value
        uint8_t before_imply = np->value_5;

        /* Fwd_Imp */
        #ifdef DALG_DEBUG_INFO_ON
        cout << ">>>>>> Foward imply for node: " << np->num << endl;
        #endif
        switch(np->type)
        {
            case e_gtype::IPT:
                // Do nothing
                break;

            case e_gtype::BRCH:
                unode_val5 = np->unodes[0]->value_5;
                new_val5 = unode_val5;
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;

            case e_gtype::NOT:
                unode_val5 = np->unodes[0]->value_5;
                new_val5 = ~unode_val5;
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;

            case e_gtype::AND:
                new_val5 = 0xFF;
                for(node* unode: np->unodes){
                    new_val5 = unode->value_5 & new_val5;
                }
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;

            case e_gtype::NAND:
                new_val5 = 0xFF;
                for(node* unode: np->unodes){
                    new_val5 = unode->value_5 & new_val5;
                }
                new_val5 = ~new_val5;
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;

            case e_gtype::OR:
                new_val5 = 0x00;
                for(node* unode: np->unodes){
                    new_val5 = unode->value_5 | new_val5;
                }
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;

            case e_gtype::NOR:
                new_val5 = 0x00;
                for(node* unode: np->unodes){
                    new_val5 = unode->value_5 | new_val5;
                }
                new_val5 = ~new_val5;
                if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                {
                    new_val5 = LOGIC_X;
                }
                UpdnC_val5(np,new_val5);
                break;


            case e_gtype::XOR:
                new_val5 = 0x00;
                for(node* unode: np->unodes){
                    if(unode->value_5 == LOGIC_X || unode->value_5 == LOGIC_XX)
                    {
                        new_val5 = LOGIC_X;
                        break;
                    }
                    new_val5 = unode->value_5 ^ new_val5;
                    if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn))
                    {
                        new_val5 = LOGIC_X;
                    }
                }
                UpdnC_val5(np,new_val5);
                break;

            default:
                cout << "Unkown gate type " << int(np->type) << " find in Fwd_Imp" << endl;
                break;
        }
        #ifdef DALG_DEBUG_INFO_ON
        // std::cout<<"=> node FWD IMPLY "<<np->num<<" value change "<<np->logic_5_to_char(before_imply);
        // std::cout<<"->"<<np->logic_5_to_char(np->value_5)<<std::endl;
        #endif 

        if(conflict){
            //std::cout<<"----FWD----> current node is "<<np->num<<" with conflict!"<<std::endl;
            conflict = false; // reset for next UpdnC
            return false;
        }
        /* Fwd_Imp END */
        #ifdef DALG_DEBUG_INFO_ON
        cout << ">>>>>> Backward imply for node: " << np->num << endl;
        #endif
        //if (np->num==266) np->pn();
        /* Bck_Imp */

        node* unode = NULL;
        before_imply = np->value_5;
        switch (np->type)
        {
        
            case e_gtype::IPT:
                // Do nothing
                break;

            case e_gtype::BRCH:
                unode = np->unodes[0];
                if(np->value_5 == LOGIC_D){
                    new_val5 = LOGIC_1;
                }
                else if(np->value_5 == LOGIC_Dn){
                    new_val5 = LOGIC_0;
                }
                else{
                    new_val5 = np->value_5;
                }
                UpdnC_val5(unode,new_val5);
                break;
            
            case e_gtype::NOT:
                unode = np->unodes[0];
                if(np->value_5 == LOGIC_D){
                    new_val5 = LOGIC_0;
                }
                else if(np->value_5 == LOGIC_Dn){
                    new_val5 = LOGIC_1;
                }
                else{
                    new_val5 = ~(np->value_5);
                }
                UpdnC_val5(unode,new_val5);
                break;
            
            case e_gtype::AND:
                // Output np's value is 1 or D
                // All input must be uncontrol value 1
                if(np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D){
                    new_val5 = LOGIC_1;
                    for(node* unode: np->unodes)
                    {
                        //std::cout<<"_________ updating node "<<unode->num<<std::endl;
                        UpdnC_val5(unode,new_val5);
                    }
                }
                // Output np's value is 0 or Dn
                else if(np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn){
                    // Input has control value, then do nothing
                    if(in_0_cnt != 0)
                    {
                        //std::cout<<"_________ updating node "<<np->num<<" out value 0 for AND BCK WRD"<<std::endl;
                    }
                    else if(in_0_cnt == 0)
                    {
                        // input currently have no control value 0, but only one input is X
                        // imply that input with X must be control value 0
                        if(in_x_cnt==1){
                            node* fin_with_x = in_x_vec[0];
                            new_val5 = LOGIC_0;
                            UpdnC_val5(fin_with_x,new_val5);
                        }
                        // Otherwise if multiple inputs with X exist, it is a J_fnter
                        else{
                            // J_fnter[np];
                        }
                        
                    }
                }
                else if((np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX))
                {
                    // myself is unknown, do nothing in Bck_Imp
                    
                }
                else{
                    cout << "############# ERROR missing case in Bck_Imp AND Gate " << endl;
                    cout << "Output value is " << np->logic_5_to_char(np->value_5) << endl;
                    cout << "Input value are: ";
                    for(node* unode:np->unodes){
                        cout<<np->logic_5_to_char(unode->value_5)<< " ";
                    }
                    cout << endl;
                }
                break;

            case e_gtype::NAND:
                // Output np's value is 0 or Dn
                // All input must be uncontrol value 1
                if(np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn){
                    new_val5 = LOGIC_1;
                    for(node* unode: np->unodes){
                        UpdnC_val5(unode,new_val5);
                    }
                }
                // Output np's value is 1 or D
                else if(np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D){
                    // Input has control value, then do nothing
                    if(in_0_cnt != 0){
                        // nothing
                    }
                    else if(in_0_cnt == 0){
                        // input currently have no control value 0, but only one input is X
                        // imply that input with X must be control value 0
                        if(in_x_cnt==1){
                            node* fin_with_x = in_x_vec[0];
                            new_val5 = LOGIC_0;
                            UpdnC_val5(fin_with_x,new_val5);
                        }
                        // Otherwise if multiple inputs with X exist, it is a J_fnter
                        else{
                            // J_fnter[np];
                        }
                        
                    }
                }
                else if((np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX)){
                    // myself is unknown, do nothing in Bck_Imp
                    
                }
                else{
                    cout << "############# ERROR missing case in Bck_Imp NAND Gate " << endl;
                    cout << "Output value is " << np->logic_5_to_char(np->value_5) << endl;
                    cout << "Input value are: ";
                    for(node* unode:np->unodes){
                        cout<<np->logic_5_to_char(unode->value_5)<< " ";
                    }
                    cout << endl;
                }
                break;
            
            case e_gtype::OR:
                // Output np's value is 0 or Dn
                // All input must be uncontrol value 0
                if(np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn){
                    new_val5 = LOGIC_0;
                    for(node* unode: np->unodes){
                        UpdnC_val5(unode,new_val5);
                    }
                }
                // Output np's value is 1 or D
                else if(np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D){
                    // Input has control value, then do nothing
                    if(in_1_cnt != 0){
                        // nothing
                    }
                    else if(in_1_cnt == 0){
                        // input currently have no control value 1, but only one input is X
                        // imply that input with X must be control value 1
                        if(in_x_cnt==1){
                            node* fin_with_x = in_x_vec[0];
                            new_val5 = LOGIC_1;
                            UpdnC_val5(fin_with_x,new_val5);
                        }
                        // Otherwise if multiple inputs with X exist, it is a J_fnter
                        else{
                            // J_fnter[np];
                        }
                        
                    }
                }
                else if((np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX)){
                    // myself is unknown, do nothing in Bck_Imp
                    
                }
                else{
                    cout << "############# ERROR missing case in Bck_Imp OR Gate " << endl;
                    cout << "Output value is " << np->logic_5_to_char(np->value_5) << endl;
                    cout << "Input value are: ";
                    for(node* unode:np->unodes){
                        cout<<np->logic_5_to_char(unode->value_5)<< " ";
                    }
                    cout << endl;
                }
                break;

            case e_gtype::NOR:
                // Output np's value is 1 or D
                // All input must be uncontrol value 0
                if(np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D){
                    new_val5 = LOGIC_0;
                    for(node* unode: np->unodes){
                        UpdnC_val5(unode,new_val5);
                    }
                }
                // Output np's value is 0 or Dn
                else if(np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn){
                    // Input has control value, then do nothing
                    if(in_1_cnt != 0){
                        // nothing
                    }
                    else if(in_1_cnt == 0){
                        // input currently have no control value 1, but only one input is X
                        // imply that input with X must be control value 1
                        if(in_x_cnt==1){
                            node* fin_with_x = in_x_vec[0];
                            new_val5 = LOGIC_1;
                            UpdnC_val5(fin_with_x,new_val5);
                        }
                        // Otherwise if multiple inputs with X exist, it is a J_fnter
                        else{
                            // J_fnter[np];
                        }
                        
                    }
                }
                else if((np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX)){
                    // myself is unknown, do nothing in Bck_Imp
                    
                }
                else{
                    cout << "############# ERROR missing case in Bck_Imp NOR Gate " << endl;
                    cout << "Output value is " << np->logic_5_to_char(np->value_5) << endl;
                    cout << "Input value are: ";
                    for(node* unode:np->unodes){
                        cout<<np->logic_5_to_char(unode->value_5)<< " ";
                    }
                    cout << endl;
                }
                break;

            case e_gtype::XOR:
                // As long as two X at input, cannot backward imply this XOR gate
                // And it is a J_fnter
                if(in_x_cnt >= 2){
                    // J_fnter[np];
                }
                else if(in_x_cnt == 1){
                    node* fin_with_x = in_x_vec[0];
                    new_val5 = LOGIC_X;
                
                    // XOR do not care 0 & dn
                    int parity_1 = in_1_cnt & 0x01;
                    int parity_d = in_d_cnt & 0x01;
                    parity_1 = parity_1 ^ parity_d;
                    if((np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D) && parity_1 == 0){
                        new_val5 = LOGIC_1;
                    }
                    else if((np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D) && parity_1 == 1){
                        new_val5 = LOGIC_0;
                    }
                    else if((np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn) && parity_1 == 0){
                        new_val5 = LOGIC_0;
                    }
                    else if((np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn) && parity_1 == 1){
                        new_val5 = LOGIC_1;
                    }
                    else if((np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX)){
                        // myself is unknown, do nothing in Bck_Imp  
                                          
                    }
                    else{
                        cout << "############# ERROR missing case in Bck_Imp XOR Gate " << endl;
                        cout << "Output value is " << np->logic_5_to_char(np->value_5) << endl;
                        cout << "Input value are: ";
                        for(node* unode:np->unodes){
                            cout<<np->logic_5_to_char(unode->value_5)<< " ";
                        }
                        cout << endl;
                        np->pn();
                    }
                    UpdnC_val5(fin_with_x,new_val5);
                }
                else{
                    // input has no X, do nothing
                }
                break;

            default:
                cout << "Unkown gate type " << int(np->type) << " find in Bck_Imp" << endl;
                break;
        }
        #ifdef DALG_DEBUG_INFO_ON
        std::cout<<"+++++++++++++ finished "<< np->num<<" all implication!"<<endl;

        // std::cout<<"=> node BCK IMPLY "<<np->num<<" value change "<<np->logic_5_to_char(before_imply);
        // std::cout<<"->"<<np->logic_5_to_char(np->value_5)<<std::endl;;
        #endif

        if(conflict){
            conflict = false; // reset for next UpdnC
            return false;
        }
    }
    // Imply and Check finish when ImpnC_stk empty
    // Update D_fnter and J_fnter
    Find_D_J_fnter();
    // Return true if no conflict found
    #ifdef DALG_DEBUG_INFO_ON
    std::cout<<"return true for IMP+++++++"<<std::endl;
    #endif
    return true;
}

void circuit:: UpdnC_val5 (node* target_np, uint8_t new_val5){

    if(!(new_val5 == LOGIC_0 || new_val5 == LOGIC_1 || new_val5 == LOGIC_D || new_val5 == LOGIC_Dn)){
        new_val5 = LOGIC_X;
    }

    #ifdef DALG_DEBUG_INFO_ON
    std::cout<<"    ==== node "<<target_np->num<<" original value is "<<target_np->logic_5_to_char(target_np->value_5)<<" and new value is "<<target_np->logic_5_to_char(new_val5)<<std::endl;
    #endif

    if ((target_np->value_5 == LOGIC_X || target_np->value_5 == LOGIC_XX) && (new_val5 !=LOGIC_X && new_val5 != LOGIC_XX)){
        target_np->value_5 = new_val5;
        push_stk(target_np); // if value5 changes from X to other, push corresponding nodes to stack
        conflict = false;
    }
    else if (target_np->value_5 == LOGIC_D && new_val5 == LOGIC_1) {
        conflict = false;
    }
    else if (target_np->value_5 == LOGIC_Dn && new_val5 == LOGIC_0) {
        conflict = false;
    }
    else if (target_np->value_5 == LOGIC_1 && new_val5 == LOGIC_D) {
        conflict = false;
    }
    else if (target_np->value_5 == LOGIC_0 && new_val5 == LOGIC_Dn) {
        conflict = false;
    }
    else if (new_val5 == LOGIC_X || new_val5 == LOGIC_XX) {
        conflict = false;
    }
    else if (new_val5 == target_np->value_5) {
        conflict = false;
    }

    else if (target_np->value_5 == LOGIC_1 && new_val5 == LOGIC_0) {
        conflict = true;
    }
    else if (target_np->value_5 == LOGIC_0 && new_val5 == LOGIC_1) {
        conflict = true;
    }
    else if (target_np->value_5 == LOGIC_D && new_val5 == LOGIC_0) {
        conflict = true;
    }
    else if (target_np->value_5 == LOGIC_Dn && new_val5 == LOGIC_1) {
        conflict = true;
    }
    else if (target_np->value_5 == LOGIC_0 && new_val5 == LOGIC_D) {
        conflict = true;
    }
    else if (target_np->value_5 == LOGIC_1 && new_val5 == LOGIC_Dn) {
        conflict = true;
    }
    else
    {
        cout << "############# ERROR missing case in UpdnC " << endl;
        cout << "new_val5 is: " << target_np->logic_5_to_char(new_val5) << endl;
        cout << "target_np->value_5 is: " << target_np->logic_5_to_char(target_np->value_5) << endl;
    }
    
    
    #ifdef DALG_DEBUG_INFO_ON
    if (conflict == true) {
        cout << "Conflict find at node: " << endl;
        target_np->pn();
        cout << "old_val5 is: " << target_np->logic_5_to_char(target_np->value_5) << endl;
        cout << "new_val5 is: " << target_np->logic_5_to_char(new_val5) << endl;

        // std::cout<<"===================="<<std::endl<<"!!! D-F size is "<< this->D_fnter.size()<<" and J-F size is "<< this->J_fnter.size()<<std::endl;
        // std::cout<<"===================="<< "!!! D-F content is " << std::endl;
        // for(auto iter:D_fnter){
        // cout << iter.first->num << " " << iter.second << endl; 
        //}
    }
    #endif

    return;
}

// helper function to update Fwd_stk or Bck_stk depend on doing Fwd or Bck imply
void circuit:: push_stk (node* np){
    #ifdef DALG_DEBUG_INFO_ON
    std::cout<<"        ----pushing the IO of "<<np->num<<" into stack with its value "<<np->logic_5_to_char(np->value_5)<<std::endl;
    #endif
    // for foward imply
    for(node* dnode: np->dnodes){
        ImpnC_stk.push(dnode);
        #ifdef DALG_DEBUG_INFO_ON
        std::cout<<"        push "<<dnode->num<<" into stack"<<std::endl;
        #endif
    }
    // for backward imply
    ImpnC_stk.push(np);
    
}

void circuit:: set_ckt_val5(int node_num, int fault){
    for(node* np: Node){
        if(np->num == node_num){
            if(fault == 1){
                np->value_5=LOGIC_X;
                np->XOR_D_fnter_c_cbar=false;
                UpdnC_val5(np,LOGIC_Dn);
            }
            else if(fault == 0){
                np->value_5=LOGIC_X;
                np->XOR_D_fnter_c_cbar=false;
                UpdnC_val5(np,LOGIC_D);
            }
            else{
                cout << "fault should be either 1 or 0, in 'set_ckt_val5' " << endl;
            }
        }
        else{
            np->XOR_D_fnter_c_cbar=false;
            np->value_5 = LOGIC_X;
        }
    }
    #ifdef DALG_DEBUG_INFO_ON
    std::cout<<"setting node "<<this->Node[this->num_to_idx[node_num]]->num<<" with value "<<this->Node[this->num_to_idx[node_num]]->logic_5_to_char(this->Node[this->num_to_idx[node_num]]->value_5)<<std::endl;
    #endif
}

void circuit:: Find_D_J_fnter(){

    #ifdef DALG_DEBUG_INFO_ON
    std::cout<<"------------ inside FIND DJ FNTR"<<std::endl;
    #endif

    this->D_fnter.clear();
    this->J_fnter.clear();
    
    for(node* np: circuit::Node){ // current node of interation is np
        
        vector<node*> in_0_vec, in_1_vec, in_x_vec, in_d_vec, in_dn_vec;
        
        for(node* fin_node: np->unodes) // upnodes information of current node
        {
            switch(fin_node->value_5)
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
                case LOGIC_X:
                case LOGIC_XX:
                    in_x_vec.push_back(fin_node);
                    break;
            }
        }

        int in_0_cnt = in_0_vec.size();
        int in_1_cnt = in_1_vec.size();
        int in_d_cnt = in_d_vec.size();
        int in_dn_cnt = in_dn_vec.size();
        int in_x_cnt = in_x_vec.size();
        int in_total_cnt = np->fin;
        
        switch(np->type)
        {
            case e_gtype::IPT:

                break;

            case e_gtype::BRCH:
               
                break;

            case e_gtype::NOT:
               
                break;

            case e_gtype::AND:
              
                if(np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX) { // find D_fnter
                    // D_fnter case 1
                    // input has D, input do not have Dn, input do not have control value 0, input has X,
                    if (in_d_cnt !=0 && in_dn_cnt ==0 && in_0_cnt == 0 && in_x_cnt != 0 ) {
                        D_fnter[np];
                    }
                    // D_fnter case 2
                    // input has Dn, input do not have D, input do not have control value 0, input has X,
                    else if (in_dn_cnt !=0 && in_d_cnt ==0 && in_0_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                }
                
                if((np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn)  && in_0_cnt == 0 && in_x_cnt >= 2){ // find J_fnter
                    if(!((in_d_cnt!=0 && in_dn_cnt ==0) || (in_d_cnt==0 && in_dn_cnt !=0))){
                        J_fnter[np];
                    }
                }
                
                
                break;

            case e_gtype::NAND:
                #ifdef DALG_DEBUG_INFO_ON
                std::cout<<"~~~~~~~=>searching D/J-F for node "<<np->num<<"-"<<np->logic_5_to_char(np->value_5)<<" with its input 0-"<<in_0_cnt<<" 1-"<<in_1_cnt<<" D-"<<in_d_cnt<<" N-"<<in_dn_cnt<<" X-"<<in_x_cnt<<std::endl;
                #endif

                if(np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX){
                    // D_fnter case 1
                    // input has D, input do not have Dn, input do not have control value 0, input has X,
                    if (in_d_cnt !=0 && in_dn_cnt ==0 && in_0_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                    // D_fnter case 2
                    // input has Dn, input do not have D, input do not have control value 0, input has X,
                    else if (in_dn_cnt !=0 && in_d_cnt ==0 && in_0_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                }
                
                if((np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D) && in_0_cnt == 0 && in_x_cnt >= 2){
                    if(!((in_d_cnt!=0 && in_dn_cnt ==0) || (in_d_cnt==0 && in_dn_cnt !=0))){
                        J_fnter[np];
                    }
                }
                
                break;

            case e_gtype::OR:
                if(np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX){
                    // D_fnter case 1
                    // input has D, input do not have Dn, input do not have control value 1, input has X,
                    if (in_d_cnt !=0 && in_dn_cnt ==0 && in_1_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                    // D_fnter case 2
                    // input has Dn, input do not have D, input do not have control value 1, input has X,
                    else if (in_dn_cnt !=0 && in_d_cnt ==0 && in_1_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                }
                if((np->value_5 == LOGIC_1 || np->value_5 == LOGIC_D) && in_1_cnt == 0 && in_x_cnt >= 2){
                    if(!((in_d_cnt!=0 && in_dn_cnt ==0) || (in_d_cnt==0 && in_dn_cnt !=0))){
                        J_fnter[np];
                    }
                }
                
                break;

            case e_gtype::NOR:
                if(np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX){
                    // D_fnter case 1
                    // input has D, input do not have Dn, input do not have control value 1, input has X,
                    if (in_d_cnt !=0 && in_dn_cnt ==0 && in_1_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                    // D_fnter case 2
                    // input has Dn, input do not have D, input do not have control value 1, input has X,
                    else if (in_dn_cnt !=0 && in_d_cnt ==0 && in_1_cnt == 0 && in_x_cnt != 0) {
                        D_fnter[np];
                    }
                }
                
                if((np->value_5 == LOGIC_0 || np->value_5 == LOGIC_Dn) && in_1_cnt == 0 && in_x_cnt >= 2){
                    if(!((in_d_cnt!=0 && in_dn_cnt ==0) || (in_d_cnt==0 && in_dn_cnt !=0))){
                        J_fnter[np];
                    }
                }

                break;


            case e_gtype::XOR:
                int parity_1 = in_1_cnt & 0x01;
                int parity_d = in_d_cnt & 0x01;
                int parity_dn = in_dn_cnt & 0x01;
                    // D_fnter case 1
                    // input with d does not cancel itself (parity==1), input with dn cancel itself, input with 1 cancel it self, input has value with X
                if(np->value_5 == LOGIC_X || np->value_5 == LOGIC_XX){
                    if (parity_d != parity_dn  &&  in_x_cnt != 0) {
                        D_fnter[np];
                    }
                }
                
                if(np->value_5 == LOGIC_1 || np->value_5 == LOGIC_0 || np->value_5 == LOGIC_D || np->value_5 == LOGIC_Dn){
                    if(in_x_cnt >= 2){
                        J_fnter[np];
                    }
                }
                            
                break;
                
                
        }    
        
    }
    
}

bool circuit:: err_at_PO(){
    for(node* PO: Poutput){
        if(PO->value_5 == LOGIC_D || PO->value_5 == LOGIC_Dn){
            return true;
        }
    }
    return false;
}

void circuit:: save_to_stk(){
    ckt_snapsht* curr_ckt = new ckt_snapsht();
    for(node* np: Node)
        curr_ckt->node_val5_copy[np->num] = np->value_5;
    curr_ckt->D_fnter_copy = D_fnter;
    curr_ckt->J_fnter_copy = J_fnter;
    ckt_rec_stk.push(curr_ckt);
}

void circuit:: rec_from_stk(){
    // first clear D_fnter and J_fnter
    this->D_fnter.clear();
    this->J_fnter.clear();

    assert(!ckt_rec_stk.empty());
    // if(!ckt_rec_stk.empty())
    // {

        ckt_snapsht* rec_ckt = ckt_rec_stk.top();
        ckt_rec_stk.pop();

        this->D_fnter = rec_ckt->D_fnter_copy;
        this->J_fnter = rec_ckt->J_fnter_copy;

        for(node* np: Node){
            np->value_5 = rec_ckt->node_val5_copy[np->num];
        }

        delete rec_ckt;

    //}
}

bool circuit:: all_D_fnter_tried(){
    for(auto iter: this->D_fnter){
        if(iter.second == 0){
            return false;
        }
    }
    return true;
}

void circuit:: assign_cbar_to_D_fnter_fin(node* curr_D_fnter){
    
    uint8_t c_bar;

    switch(curr_D_fnter->type)
    {

    case e_gtype::IPT:
        break;

    case e_gtype::BRCH:
        break;

    case e_gtype::NOT:
        break;

    case e_gtype::AND:
        c_bar = LOGIC_1;
        for(node* unode: curr_D_fnter->unodes){
            if(unode->value_5 == LOGIC_X || unode->value_5 ==  LOGIC_XX){
                UpdnC_val5(unode, c_bar);
            }
        }
        break;
    
    case e_gtype::NAND:
        c_bar = LOGIC_1;
        for(node* unode: curr_D_fnter->unodes){
            if(unode->value_5 == LOGIC_X || unode->value_5 ==  LOGIC_XX){
                UpdnC_val5(unode, c_bar);
            }
        }
        break;
    
    case e_gtype::OR:
        c_bar = LOGIC_0;
        for(node* unode: curr_D_fnter->unodes){
            if(unode->value_5 == LOGIC_X || unode->value_5 ==  LOGIC_XX){
                UpdnC_val5(unode, c_bar);
            }
        }
        break;

    case e_gtype::NOR:
        c_bar = LOGIC_0;
        for(node* unode: curr_D_fnter->unodes){
            if(unode->value_5 == LOGIC_X || unode->value_5 ==  LOGIC_XX){
                UpdnC_val5(unode, c_bar);
            }
        }
        break;
    
    case e_gtype::XOR:
        if(!curr_D_fnter->XOR_D_fnter_c_cbar){
            c_bar = LOGIC_0;
            curr_D_fnter->XOR_D_fnter_c_cbar = true;
            D_fnter[curr_D_fnter] = 0; // need to assign 1 to XOR D_fnter again
        }
        else if(curr_D_fnter->XOR_D_fnter_c_cbar){
            c_bar = LOGIC_1;
            curr_D_fnter->XOR_D_fnter_c_cbar = false;
            D_fnter[curr_D_fnter] = 1; // both 1 and 0 had tried for this D_fnter
        }
        else{
            cout << "XOR D_fnter check unexpected case" << endl;
        }
        for(node* unode: curr_D_fnter->unodes){
            if(unode->value_5 == LOGIC_X || unode->value_5 ==  LOGIC_XX){
                UpdnC_val5(unode, c_bar);
            }
        }
        break;
    
    default:
        cout << "Unkown case find in 'assign_cbar_to_D_fnter_fin' " << endl;
        break;
    }
    
}

bool circuit:: all_inputs_of_curr_J_fnter_specified(node* curr_J_fnter){
    for(node* unode: curr_J_fnter->unodes){
        if(unode->value_5==LOGIC_XX || unode->value_5==LOGIC_X){
            return false;
        }
    }
    return true;
}

void circuit:: get_c_cbar_for_curr_J_fnter(node* curr_J_fnter, uint8_t &c, uint8_t &c_bar){
    switch (curr_J_fnter->type)
    {

    case e_gtype::AND:
        c = LOGIC_0;
        break;
    
    case e_gtype::NAND:
        c = LOGIC_0;
        break;

    case e_gtype::OR:
        c = LOGIC_1;
        break;

    case e_gtype::NOR:
        c = LOGIC_1;
        break;

    case e_gtype::XOR:
        c = LOGIC_0;
        break;
    
    default:
        cout << "unkown gate find in 'get_c_cbar_for_curr_J_fnter' " << endl; 
        break;
    }
    c_bar = ~c;
}

void circuit:: save_DALG_rst(vector<node*> Pinput){
    DALG_rst_num.clear();
    DALG_rst_val5.clear();
    for(node* PI: Pinput){
        DALG_rst_num.push_back(PI->num);
        DALG_rst_val5.push_back(PI->value_5);
    }
}

bool circuit:: ATPG_rst_check(string TP_in, string sDFS_fout, int node_num_in_test, int fault_in_test){

    this->DFS_continuous(TP_in, sDFS_fout);

    string line;
    unordered_map<int, int> DFS_faults;
    

    fstream fp;
    fp.open(sDFS_fout,ios::in);
    assert(fp);

    while(getline(fp,line)){

        string node_num="";
        string fault="";

        stringstream ss(line);
        char tmp;

        while(ss>>tmp){
            if(isdigit(tmp)){
                node_num += tmp; 
            }
            else if(!isdigit(tmp)){
                break; // "@" find, do nothing
            }
        }
        while(ss>>tmp){
            fault += tmp;
        }

        int node_num_int = std::stoi(node_num);
        int fault_int = std::stoi(fault);
        DFS_faults[node_num_int] = fault_int;
    }

    fp.close();

    if(DFS_faults[node_num_in_test]==fault_in_test){
        return true;
    }
    else{
        return false;
    }
    
}

void circuit:: set_DALG_timeout_coefficient(){
    if(this->Nnodes < DALG_TO_LIMIT_1){
        this->DALG_TO_coeff = DALG_FIRST_COEFF;
    }
    else if(DALG_TO_LIMIT_1 < this->Nnodes < DALG_TO_LIMIT_2){
        this->DALG_TO_coeff = DALG_SECOND_COEFF;
    }
    else if(DALG_TO_LIMIT_2 < this->Nnodes < DALG_TO_LIMIT_3){
        this->DALG_TO_coeff = DALG_THIRD_COEFF;
    }
    else{
        this->DALG_TO_coeff = DALG_FOURTH_COEFF;
    }
}

void circuit:: DALG_reset(){
    // clear circuit recovery stack
    while(!this->ckt_rec_stk.empty())
    {
        delete this->ckt_rec_stk.top();
        this->ckt_rec_stk.pop();
    }

    while(!this->ImpnC_stk.empty())
    {
        this->ImpnC_stk.pop();
    }

    this->D_fnter.clear();
    this->J_fnter.clear();

    // Reset DALG_rst vector
    DALG_rst_num.clear();
    DALG_rst_val5.clear();

    // Reset DALG counter
    DALG_cnt = 0;
}
