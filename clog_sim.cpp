#include "circuit.hpp"
//#include "node.hpp"

void circuit::log_sim(string log_file_name){
    char tmp;

    //set val of all Node to -1 (initial value of each node in circuit is -1)
    for (int i = 0; i < Nnodes; i++){
        np = Node[i];
        np -> val = -1;
    }
    

    //read the primary input file, if number(in input file) = Pinput -> num, val = value(in input file)
    fin.open(log_file_name, ios::in);
    
    while (getline(fin, log_in)) {
        stringstream col(log_in);
       
        col>>log_num;
        col>>tmp;
        col>>log_val;
       // cout<<"log_num = "<<log_num<<endl;
        for(int i = 0; i < Npi; i++){
            np = Pinput[i];
            //cout<<"num = "<<np->num<<endl;
            if(np->num == log_num){
                np->val = log_val;
                break;
               // cout << "Primary num and val is "<<np->num<<","<<np->val<<endl;
            }// if read file line successful
        } // for
    } // while
    fin.close();
   
    // find the max lvl from all Poutput
    
    int sum = 0; //in xor gate


    // for(int i = 0; i<=max_lvl; i++){
    //     cout<<"level = "<<i;
    //     for(auto x : table[i]){
    //         cout<<"-->"<<x->num;
    //     }
    //     cout<<endl;
    // }
    
    
    // for all node which lvl is j (from 1 to max lvl)
        // if type = 1 (branch)  val = unodes[0] -> val
        // if type = 6 (NAND)  if undoes[i(from 0 to fin-1)]=0, val = 1, otherwise val =0 (control value = 0)
        // ...... other type
    for (int j = 1; j <= this->max_lvl; j++){ // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
        for(auto x : this->table[j]){
            np = x;
          
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
                        
                    case (e_gtype::OR):
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 1){
                                np->val = 1;
                                break;
                            }
                            else np->val = 0;
                        } // for k
                        //cout<<"it's OR"<<endl;
                        break;
                        
                    case (e_gtype::NOR):
                        for (int k = 0 ; k < np->fin; k ++){
                            if(np->unodes[k]->val == 1){
                                np->val = 0;
                                break;
                            }
                            else np->val = 1;
                        } // for k
                        //cout<<"it's NOR"<<endl;
                        break;
                    
                    case (e_gtype::NOT):
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
                    
                    default: 
                        cout<<"it's not valid type !"<<endl;
                        break;
                        

                    
                       
                } // swith type
            
        } // for i < Nnodes
    }// for j < max_lvl
    // if (this->table != NULL) {
    //     delete[] this->table;
    // }
    
     //using to debug
    // for(int i = 0; i < Nnodes; i++){
    //      np = Node[i];
    //      cout << np->num <<","<< np->val << endl;
    //  }
    
    this->simulation_flag = 1;

}



void circuit::log_sim_5(string log_file_name)
{
    log_sim_5_result.erase(log_sim_5_result.begin(),log_sim_5_result.end());
    //cout<<"length of result array"<<log_sim_5_result.size()<<endl;
    vector<vector<char>> TP_vector = log_read_TP(log_file_name);

    char tmp;
    char logic_bit;

    int cnt_0, cnt_1, cnt_x, cnt_d, cnt_dn;

    for(int i = 0; i < Nnodes; i++)
    {
        this->Node[i]->value_5 = LOGIC_X;
    }


    // find the max lvl from all Poutput
    //int max_lvl = 0;
    int sum = 0; //in xor gate
    // for (int i = 0; i < Npo; i++){
    //     np = Poutput[i];
    //     if (np->lvl > max_lvl){
    //         max_lvl = np->lvl;
    //     }
    // }
    
    for (vector<char> TP: TP_vector)
    {
        vector<char> log_sim_5_res_row; // result vector for store one TP
        for(node* np: Pinput)
        {
            logic_bit = TP[np->TP_idx];
            //std::cout<<"TP_idx is "<<np->TP_idx<<" and its index is "<<TP[np->TP_idx]<<" and its val is "<<TP[np->TP_idx]<<std::endl;
            logic_bit = isalpha(logic_bit) ? tolower(logic_bit) : logic_bit;

                switch(logic_bit)
                {
                    case '0':
                        np->value_5 = LOGIC_0;
                        break;
                    case '1':
                        np->value_5 = LOGIC_1;
                        break;
                    case 'x':
                        np->value_5 = LOGIC_X;
                        break;
                    case 'd':
                        np->value_5 = LOGIC_D;
                        break;
                    case 'n':
                        np->value_5 = LOGIC_Dn;
                        break;
                    default:
                        printf("the char with ascii %d and char '%c' is not legal!\n", logic_bit, logic_bit);
                        break;                  
                }
            
            //cout << "Primary num and val is "<<np->num<<","<<np->val<<endl;
        }
    
    
       
    // for all node which lvl is j (from 1 to max lvl)
        // if type = 1 (branch)  val = unodes[0] -> val
        // if type = 6 (NAND)  if undoes[i(from 0 to fin-1)]=0, val = 1, otherwise val =0 (control value = 0)
        // ...... other type
    for (int j = 1; j <= max_lvl; j++){ // lev = 1, 2 ... max_lev
        //cout<<"level is:"<<j<<endl;
        for(int i = 0; i < Nnodes; i++){
            np = Node[i];
            if (np->lvl == j){

                switch(np->type){
                        
                    case (e_gtype::BRCH):
                        np->value_5 = np->unodes[0]->value_5;
                        //cout<<"it's branck"<<endl;
                        break;
                    
                    case (e_gtype::XOR):
                        tmp = 0;
                        for(node* fan_in_node:np->unodes)
                        {
                            tmp = tmp ^ fan_in_node->value_5;
                        }
                        if(tmp == LOGIC_X || tmp == LOGIC_XX)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = tmp;
                        }
                        break;
                        
                    case  (e_gtype::OR):
                        tmp = 0;
                        for(node* fan_in_node:np->unodes)
                        {
                            tmp = tmp | fan_in_node->value_5;
                        }
                        if(tmp == LOGIC_X || tmp == LOGIC_XX)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = tmp;
                        }
                        break;
                        
                    case  (e_gtype::NOR):
                        tmp = 0;
                        for(node* fan_in_node:np->unodes)
                        {
                            tmp = tmp | fan_in_node->value_5;
                        }
                        if(tmp == LOGIC_X || tmp == LOGIC_XX)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = ~tmp;
                        }
                        break;
                        //cout<<"it's NOR"<<endl;
                    
                    case  (e_gtype::NOT):
                        if(np->unodes[0]->value_5 == LOGIC_XX || np->unodes[0]->value_5 == LOGIC_X)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = ~np->unodes[0]->value_5;
                        }
                        
                        //cout<<"it's NOT"<<endl;
                        break;
       
                    case (e_gtype::NAND):
                        tmp = 0xFF;
                        for(node* fan_in_node:np->unodes)
                        {
                            tmp = tmp & fan_in_node->value_5;
                        }
                        if(tmp == LOGIC_X || tmp == LOGIC_XX)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = ~tmp;
                        }
                        break;

                    
                    case  (e_gtype::AND):
                        tmp = 0xFF;
                        for(node* fan_in_node:np->unodes)
                        {
                            tmp = tmp & fan_in_node->value_5;
                        }
                        if(tmp == LOGIC_X || tmp == LOGIC_XX)
                        {
                            np->value_5 = LOGIC_X;
                        }
                        else
                        {
                            np->value_5 = tmp;
                        }
                        break;
                    
                    default: cout<<"it's not valid type !"<<endl;
                     
                    } // swith type
                } // if lvl = j
                
            } // for i < Nnodes
        }// for j < max_lvl

        
        for(node* PO: Poutput){
            //cout << static_cast<int>(PO->value_5) << ",";
            char log_5_char = PO->logic_5_to_char(PO->value_5);
            log_sim_5_res_row.push_back(log_5_char);
        }
        log_sim_5_result.push_back(log_sim_5_res_row);
        //cout << "size of log_sim_5 vector is: " << log_sim_5_result.size() << endl;
        //cout << endl;
    } // for

    

    //cout << "-------------END OF THIS ONE----------------" << endl;
     //using to debug
    // for(int i = 0; i < Nnodes; i++){
    //     np = Node[i];
    //     cout << np->num <<","<< np->val << endl;
    // }
    
    this->simulation_flag = 1;

}


void circuit::log_sim(void)
{
    int sum;
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
