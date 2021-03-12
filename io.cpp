#include "circuit.hpp"

#define DEBUG_MODE_DFS 1

string circuit:: get_gname(int tp){
    switch(tp) {
        case 0: return("PI");
        case 1: return("BRANCH");
        case 2: return("XOR");
        case 3: return("OR");
        case 4: return("NOR");
        case 5: return("NOT");
        case 6: return("NAND");
        case 7: return("AND");
    }
}

string circuit:: get_cname(string ckt_fin){
    int fidx=0,sidx=-1;
    string cname;
    reverse(ckt_fin.begin(),ckt_fin.end());
    fidx = ckt_fin.find(".");
    sidx = ckt_fin.find("/");
    if(sidx>=0){
        ckt_fin.erase(ckt_fin.begin()+sidx,ckt_fin.end());
    }
    ckt_fin.erase(ckt_fin.begin(),ckt_fin.begin()+fidx+1);
    reverse(ckt_fin.begin(),ckt_fin.end());
    cname = ckt_fin;
    return cname;
}

void circuit:: pc(){
    node* np;

    printf(" Node   Type \tIn     \t\t\tOut    \n");
    printf("------ ------\t-------\t\t\t-------\n");
    for(int i = 0; i<Nnodes; i++) {
        np = Node[i];
        printf("\t\t\t\t\t");
        for(int j = 0; j<np->fout; j++) {
            printf("%d ",np->dnodes[j]->num);
        }
        printf("\r%5d  %s\t", np->num, get_gname(static_cast<int>(np->type)).c_str());
        for(int j = 0; j<np->fin; j++) {
            printf("%d ",np->unodes[j]->num); 
        };
        printf("\n");
    }
    printf("Primary inputs:  ");
    for(int i = 0; i<Npi; i++) printf("%d ",Pinput[i]->num);
    printf("\n");
    printf("Primary outputs: ");
    for(int i = 0; i<Npo; i++) printf("%d ",Poutput[i]->num);
    printf("\n\n");
    printf("Number of nodes = %d\n", Nnodes);
    printf("Number of primary inputs = %d\n", Npi);
    printf("Number of primary outputs = %d\n", Npo);
    cout << endl;
}

void circuit:: plev(string ckt_fin){
    node* np;
    cout << "--------------levelization info-----------------" << endl;
    cout << get_cname(ckt_fin) << endl;
    cout << "#PI:" << Npi << endl;
    cout << "#PO:" << Npo << endl;
    cout << "#Nodes: " << Nnodes << endl;
    cout << "Gates: " << NGates << endl;
    for(int i=0; i<Nnodes; i++){
        np=Node[i];
        cout << np->num << "\t" << np->lvl << endl;
    } 
}

void circuit:: slev(string slev_fout, string ckt_fin){
    node* np;
    fp.open(slev_fout, ios::out);
    fp << get_cname(ckt_fin) << endl;
    fp << "#PI: " << Npi << endl;
    fp << "#PO: " << Npo << endl;
    fp << "#Nodes: " << Nnodes << endl;
    fp << "Gates: " << NGates << endl;
    for(int i=0; i<Nnodes; i++){
        np=Node[i];
        fp << np->num << "\t" << np->lvl << endl;
    } 
    fp.close();
}

// old plog format
void circuit:: plog(){
    node* np;
    cout << "--------------logic simulation info-----------------" << endl;
    for(int i = 0; i < Npo; i++){
        np = Poutput[i];
        cout << np->num <<","<< static_cast<int>(np->value_5) << endl;
    }
}
void circuit:: slog(string log_output_file){

    fp.open(log_output_file, ios::out);
    
    for(int i = 0; i < Npo; i++){
        np = Poutput[i];
        fp << np->num <<","<< np->val << endl;
        
    }
    fp.close();
       
}

void circuit:: plog(int PO_header){
    node* np;
    cout << "--------------logic simulation info-----------------" << endl;
    // PO_header: po1,po2,po3,po4 ...
    if(PO_header==0){
        for(int i=0; i<Npo; i++){
            if(i!=Npo-1){
                cout << Poutput[i]->num << ",";
            }
            else{
                cout << Poutput[i]->num;
            }
        }
        cout << endl;
        PO_header = 1;
    }
    for(vector<char> log_sim_5_res_row: log_sim_5_result){
        for(int i=0; i<log_sim_5_res_row.size();i++){
            if(i!=log_sim_5_res_row.size()-1){
                cout << log_sim_5_res_row[i] << ",";
            }
            else // last element in this row, no comma
            {
                cout << log_sim_5_res_row[i];
            }
            
        }
        cout << endl;
    }
}
void circuit:: slog(int PO_header, string output_file){
    fp.open(output_file, ios::out);
    node* np;
    // PO_header: po1,po2,po3,po4 ...
    if(PO_header==0){
        for(int i=0; i<Npo; i++){
            if(i!=Npo-1){
                fp << Poutput[i]->num << ",";
            }
            else{
                fp << Poutput[i]->num;
            }
        }
        fp << endl;
        PO_header = 1;
    }
    for(vector<char> log_sim_5_res_row: log_sim_5_result){
        for(int i=0; i<log_sim_5_res_row.size();i++){
            if(i!=log_sim_5_res_row.size()-1){
                fp << log_sim_5_res_row[i] << ",";
            }
            else // last element in this row, no comma
            {
                fp << log_sim_5_res_row[i];
            }
            
        }
        fp << endl;
    }
    fp.close();
}



// To ensure the mapping of PIs from input file to Pinput is correct, using TP_idx_map (unordered_map) to match
vector<vector<char>> circuit:: log_read_TP (string test_in){

    ifstream fin;
    fin.open(test_in,ios::in);

    string line;
    string PI_s="";
    vector<string> PI;
    int pre_col_cnt = 0;
    int cur_col_cnt = 0;
    char tmp;

    // Mapping PIs to TP_vector vector column according to node_num using TP_idx_map
    unordered_map <int,int> TP_idx_map; // Key: node_num, Value: column_num in TP_vector

    // read first line of node_num in the input file into vector, PI
    getline(fin,line);
    stringstream row(line);
    while(row>>tmp){
        if(!isdigit(tmp)){
            PI.push_back(PI_s);
            PI_s="";
        }
        else
        {
            PI_s = PI_s + tmp;
        }
    }
    PI.push_back(PI_s);

    pre_col_cnt = PI.size();

    // transfer PI's value into TP_idx_map
    // Key: node_num, Value: column_num in TP_vector vector.
    for(int idx=0; idx<PI.size(); idx++){
        TP_idx_map[stoi(PI[idx])] = idx;
    }

    //cout << "PI size is:" << PI.size() << endl;
    
    vector<vector<char>> TP_vector; // Test Pattern  
    
    while(getline(fin,line)){
        vector<char> TP_row;
        int i = 0;
        stringstream row(line);
        while(row>>tmp){
            if(!isdigit(tmp)&&!isalpha(tmp)){ // skip ","
                continue;
            }
            else{
                TP_row.push_back(tmp);
                i++;
                cur_col_cnt++;
            }
        }
        TP_vector.push_back(TP_row);
//        assert(cur_col_cnt==pre_col_cnt); // check if each row has same amount of column
        cur_col_cnt=0;
    }

    // print every element in TP_vector
    // for(auto vec: TP_vector){
    //     for(auto ele: vec){
    //         cout << ele << " ";
    //     }
    //     cout << endl;
    // }

    // associate each PI with corresponding col in TP_vector
    for(node* PI: Pinput){
        assert(TP_idx_map.count(PI->num)); // check that PI np does exist in TP_idx_map
        PI->TP_idx = TP_idx_map[PI->num];
        //cout << "PI:" << PI->num << "has idx map to:" << PI->TP_idx <<endl;
    }

    fin.close();
    return TP_vector;
}

void circuit::sDFS_once(string output_file_name)
{
    fp.open(output_file_name, ios::out);

    DFS_clause clause_sum;

#ifdef DEBUG_MODE_DFS
    for(int i = 0; i < this->Nnodes; i++)
    {
        fp<<this->Node[i]->num<<" ssa list start:"<<endl;
        for(int node_id: this->Node[i]->ssa_clause.ssa0_list)
        {  
            fp<<node_id<<"@0"<<endl;
        }
        for(int node_id: this->Node[i]->ssa_clause.ssa1_list)
        {
            fp<<node_id<<"@1"<<endl;
        }
    }
#else
    for(int i = 0; i < Npo; i++){
        // clause_sum = clause_sum + Poutput[i]->ssa_clause;
        clause_sum = clause_sum + this->Poutput[i]->ssa_clause;
        // fp << np->num <<","<< np->val << endl;
    }
    for(int node_id: clause_sum.ssa0_list)
    {
        fp<<node_id<<"@0"<<endl;
    }
    for(int node_id: clause_sum.ssa1_list)
    {
        fp<<node_id<<"@1"<<endl;
    }
#endif
    fp.close();
}



void circuit::sPFS_once(string output_file_name)
{
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

void circuit::RTG_test_pattern_report(string output_file_name)
{   cout << "output RTG test pattern to the file" << endl;
    fp.open(output_file_name, ios::out);
    for (int i = 0; i < this->RTG_input_pattern_set.size() ; ++i) { // output the test patterns to the file
        fp<<"Test pattern "<< i+1 <<endl;
        int value = RTG_input_pattern_set[i];
        for(int j = 0; j<Npi; j++){ // output the Pis according to the input pattern
            if((value/2)!=0){
                fp << Pinput[j]->num << "," << value%2 << endl;
                value/=2;
            } else{
                fp << Pinput[j]->num << "," << value << endl;
            }
        }
        fp<<endl;
    }
    fp.close();
    //fp.open(output_file_name, ios::out);
}

void circuit::RTG_FC_report(string sN_tol, string sN_TFCR, string output_file_name)
{
    int N_TFCR = stoi(sN_TFCR);
    int N_tol = stoi(sN_tol);
    cout << "output RTG fault coverage to the file" << endl;
    fp.open(output_file_name, ios::out);
    fp << "#\t" << "Fault coverage" << endl;
    for (int i = 0; i < this->Fault_coverage_record.size() ; ++i) {// output the FC to the file
        int index = 0;
        if(i == 0) index = 1;
        else if(i == this->Fault_coverage_record.size()-1) index = N_tol;
        else index = 1 + i * N_TFCR;
        fp<<setiosflags(ios::fixed)<<setprecision(2);
        fp<<index << "\t" << this->Fault_coverage_record[i]*100 << "%" <<endl;
    }
    fp.close();
}

void circuit:: sPODEM(string ckt_fin){

    string ckt_name = get_cname(ckt_fin);
    string fout = "./" + ckt_name + "/" + ckt_name + "_PODEM_" + to_string(PODEM_ssa_node) + "@" + to_string(PODEM_ssa_val) + ".txt";
    fp.open(fout, ios::out);
    // print node num in first row
    node* np;
    // print result
    for(int i=0;i<Npi;i++){
        np = Node[num_to_idx[Pinput[i]->num]];
        if(i == Npi-1) fp << np->num << endl;
        else fp << np->num << ",";
    }
    for(int i=0;i<Npi;i++){
        np = Node[num_to_idx[Pinput[i]->num]];
        if(i == Npi-1)
        {
            if(np->PODEM_val_temp == LOGIC_0 || np->PODEM_val_temp == LOGIC_Dn)
            {
                fp << "0" << endl;
            }
            else if(np->PODEM_val_temp == LOGIC_1 || np->PODEM_val_temp == LOGIC_D)
            {
                fp << "1" << endl;
            }
            else
            {
                fp << "X" << endl;
            }
        } 
        else
        {
            if(np->PODEM_val_temp == LOGIC_0 || np->PODEM_val_temp == LOGIC_Dn)
            {
                fp << "0,";
            }
            else if(np->PODEM_val_temp == LOGIC_1 || np->PODEM_val_temp == LOGIC_D)
            {
                fp << "1,";
            }
            else
            {
                fp << "X,";
            }
        }
    }
    fp.close();
}

void circuit:: sDALG(int node_num, int fault, string ckt_fin){
    string ckt_name = get_cname(ckt_fin);
    string fout = ckt_name + "_DALG_" + to_string(node_num) + "@" + to_string(fault) + ".txt";
    fp.open(fout, ios::out);
    const int DALG_size = DALG_rst_num.size();
    assert(DALG_size == DALG_rst_val5.size());
    // print node num in first row
    for(int i=0;i<DALG_size;i++){
        if(i == DALG_size-1){
            fp << DALG_rst_num[i] << endl;
        }
        else
        {
            fp << DALG_rst_num[i] << ",";
        }
    }
    node* np;
    // print result
    for(int i=0;i<DALG_size;i++){
        uint8_t curr_val_5 = DALG_rst_val5[i];
        if (curr_val_5 == LOGIC_D)
        {
            curr_val_5 = LOGIC_1;
        }
        else if(curr_val_5==LOGIC_Dn)
        {
            curr_val_5 = LOGIC_0;
        }
        else if(curr_val_5==LOGIC_X||curr_val_5==LOGIC_XX)
        {
            curr_val_5 = LOGIC_0;
        }

        if(i == DALG_size-1){
            fp << np->logic_5_to_char(curr_val_5) << endl;
        }
        else
        {
            fp << np->logic_5_to_char(curr_val_5) << ",";
        }
        
    }
    fp.close();
}

void circuit:: sDALG_ATPG_pattern(ofstream& dpf){
    const int DALG_size = DALG_rst_num.size();
    assert(DALG_size == DALG_rst_val5.size());
    node* np;
    // print result
    for(int i=0;i<DALG_size;i++){
        uint8_t curr_val_5 = DALG_rst_val5[i];
        if (curr_val_5 == LOGIC_D)
        {
            curr_val_5 = LOGIC_1;
        }
        else if(curr_val_5==LOGIC_Dn)
        {
            curr_val_5 = LOGIC_0;
        }
        else if(curr_val_5==LOGIC_X||curr_val_5==LOGIC_XX)
        {
            curr_val_5 = LOGIC_0;
        }

        if(i == DALG_size-1){
            dpf << np->logic_5_to_char(curr_val_5) << endl;
        }
        else
        {
            dpf << np->logic_5_to_char(curr_val_5) << ",";
        }
        
    }
}

void circuit:: sDALG_ATPG(int node_num, int fault, ofstream &TP_out){

    const int DALG_size = DALG_rst_num.size();
    assert(DALG_size == DALG_rst_val5.size());

    node* np;
    // print result
    for(int i=0;i<DALG_size;i++){
        uint8_t curr_val_5 = DALG_rst_val5[i];
        if (curr_val_5 == LOGIC_D)
        {
            curr_val_5 = LOGIC_1;
        }
        else if(curr_val_5==LOGIC_Dn)
        {
            curr_val_5 = LOGIC_0;
        }

        if(i == DALG_size-1){
            TP_out << np->logic_5_to_char(curr_val_5) << endl;
        }
        else
        {
            TP_out << np->logic_5_to_char(curr_val_5) << ",";
        }
        
    }
}



void circuit::PODEM_save_report(double run_time, string ckt_fin)
{
    std::string target_name = get_cname(ckt_fin) + "_PODEM_ATPG_report.txt";

    int succ_cnt = 0, fail_cnt = 0;
    double coverage;

    for(node* each_node: this->Node)
    {
        if(this->PODEM_coverage_map_0[each_node->num] == 1)
        {
            succ_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        if(this->PODEM_coverage_map_1[each_node->num] == 1)
        {
            succ_cnt++;
        }
        else
        {
            fail_cnt++;
        }
    }

    coverage = (double)(succ_cnt) / (double)(this->Nnodes << 1);
    coverage = coverage * 100;
    fstream PODEM_target;
    PODEM_target.open(target_name, ios::out);

    PODEM_target<<"Algorithm: PODEM"<<endl;
    PODEM_target<<"Circuit: "<<get_cname(ckt_fin)<<endl;
    PODEM_target<<setiosflags(ios::fixed)<<setprecision(2);
    PODEM_target<<"Fault Coverage: "<<coverage<<"%"<<endl;
    PODEM_target<<"Time: "<<run_time<<" Seconds"<<endl;
    PODEM_target.close();

}

void circuit::DALG_save_report(double run_time, string ckt_fin)
{
    std::string target_name = get_cname(ckt_fin) + "_DALG_ATPG_report.txt";

    int succ_cnt = 0, fail_cnt = 0;
    double coverage;

    for(node* each_node: this->Node)
    {
        if(this->DALG_coverage_map_0[each_node->num] == 1)
        {
            succ_cnt++;
        }
        else
        {
            fail_cnt++;
            //cout << each_node->num << "@0" << endl;
        }
        if(this->DALG_coverage_map_1[each_node->num] == 1)
        {
            succ_cnt++;
        }
        else
        {
            fail_cnt++;
            //cout << each_node->num << "@1" << endl;
        }
    }

    coverage = (double)(succ_cnt) / (double)(this->Nnodes << 1);
    coverage = coverage * 100;
    fstream DALG_target;
    DALG_target.open(target_name, ios::out);

    DALG_target<<"Algorithm: DALG"<<endl;
    DALG_target<<"Circuit: "<<get_cname(ckt_fin)<<endl;
    DALG_target<<setiosflags(ios::fixed)<<setprecision(2);
    DALG_target<<"Fault Coverage: "<<coverage<<"%"<<endl;
    DALG_target<<"Time: "<<run_time<<" Seconds"<<endl;
    DALG_target.close();

}
