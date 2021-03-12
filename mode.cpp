#include "mode.hpp"
#include "circuit.hpp"

#include "cmd_parser.cpp"
#include <string>
#include <fstream>

#include <time.h>


void CMD_MODE(){
    circuit *ckt_glb = NULL;
    
    std::string cmd_name;
    std::string ckt_fin;
    std::string test_fin;
    std::string slev_fout;
    std::string slog_fout;
    std::string sDFS_fout;
    std::string sPFS_fault_in;
    std::string sPFS_fout;
    std::string sRTG_N_tol;
    std::string sRTG_N_TFCR;
    std::string sRTG_test_pattern_out;
    std::string sRTG_FC_out;
    std::string sRFL_fout;
    std::string ATPG_ALGO;
    int sPODEM_node_num;
    int sPODEM_fault;

    int return_idx;

    int PODEM_iter_val = 0;


    clock_t PODEM_start_time, PODEM_end_time;
    clock_t DALG_start_time, DALG_end_time;
    double PODEM_run_time, DALG_run_time;
    ofstream dpf;

    while(1)
	{
        std::cout<< "command: ";
        std::cin>>cmd_name;
		return_idx = find_cmd(cmd_table, cmd_name);
        switch(return_idx)
        {  
            case(0):
            	if (ckt_glb != NULL)
                	delete ckt_glb;
                return;

            case (1):       // read cmd
                std::cin>>ckt_fin;
                ckt_glb = new circuit(ckt_fin);
                std::cin.clear();
                std::cin.sync();	
                break;

            case (2):       // print circuit cmd
                if(!ckt_glb->read_done())
                {
                    std::cout<<"==> circuit is not read yet!"<<std::endl;
                    std::cout<<"==>please read a circuit first!"<<std::endl;
                }
                else
                {
                    ckt_glb->pc();
                }
                std::cin.clear();
                std::cin.sync();	
                break;

            case(3):        // lev cmd
                std::cin>>slev_fout;
                if(!ckt_glb->read_done())
                {
                    std::cout<<"==> circuit is not read yet!"<<std::endl;
                    std::cout<<"==> please read a circuit first!"<<std::endl;
                }
                else
                {
                    ckt_glb->lev();
                    ckt_glb->slev(slev_fout,ckt_fin);
                }
                std::cin.clear();
                std::cin.sync();	
                break;
            
            case(4):        // plev cmd
                if(!ckt_glb->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
                    ckt_glb->plev(ckt_fin);
                }
                std::cin.clear();
                std::cin.sync();	
                break;

            case(5):        //log_sim cmd
                std::cin>>test_fin;
                std::cin>>slog_fout;
//                std::cout<< "test_fin is '" << test_fin <<"' and slog_fout is '"<< slog_fout << "'" << std::endl;
                if(!ckt_glb->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
//                	std::cout<<"start log_sim" << std::endl;
                    ckt_glb->log_sim_5(test_fin);
//                    std::cout<<"start slog" << std::endl;
                    ckt_glb->slog(0,slog_fout);
//                    std::cout<<"end slog" << std::endl;
                }
                std::cin.clear();
                std::cin.sync();	
                break;

            case(6):        // plog cmd
                if(!ckt_glb->sim_done())
                {
                    std::cout<<"==> circuit is not simulated yet!"<<std::endl;
                    std::cout<<"==> please simulate the circuit first!"<<std::endl;
                }
                else
                {
                    ckt_glb->plog();
                }
                std::cin.clear();
                std::cin.sync();	
                break;
            
            case(7):    // DFS cmd
                std::cin>>test_fin;
                std::cin>>sDFS_fout;
                if(!ckt_glb->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
                    ckt_glb->DFS_continuous(test_fin, sDFS_fout);
//                	std::cout<<"start log_sim" << std::endl;
                    // ckt_glb->log_sim(test_fin);
//                    std::cout<<"start slog" << std::endl;
                    // ckt_glb->DFS();
                    // ckt_glb->sDFS_once(sDFS_fout);
//                    std::cout<<"end slog" << std::endl;
                }
                std::cin.clear();
                std::cin.sync();
                break;

            case(8):    // PFS cmd
                std::cin>>test_fin;
                std::cin>>sPFS_fault_in;
                std::cin>>sPFS_fout;
                if(!ckt_glb->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
                    // ckt_glb->PFS_reset();
                    // ckt_glb->PFS_load_input(test_fin);
                    // ckt_glb->PFS_load_fault(sPFS_fault_in);
                    // ckt_glb->PFS_simulate();
                    // ckt_glb->sPFS_once(sPFS_fout);
                    ckt_glb->PFS_continuous(test_fin, sPFS_fault_in, sPFS_fout);
                }
                std::cin.clear();
                std::cin.sync();
                break;

            case(9):    // RFL cmd
                std::cin>>sRFL_fout;
                if(!ckt_glb->read_done())
                {
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }
                else
                {
                	std::cout<<"start RFL" << std::endl;
                    ckt_glb->RFL(sRFL_fout);
//                    std::cout<<"start RFL reduce" << std::endl;
//                    ckt_glb->RFL_reduce();
//                    std::cout<<"end RFL" << std::endl;
                }
                std::cin.clear();
                std::cin.sync();
                break;

            case(10):    // RTG cmd
                std::cin>>sRTG_N_tol;
                std::cin>>sRTG_N_TFCR;
                std::cin>>sRTG_test_pattern_out;
                std::cin>>sRTG_FC_out;

                if(!ckt_glb->read_done())
                {
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }
                else
                {
                    std::cout<<"start RTG" << std::endl;
                    ckt_glb->RTG(sRTG_N_tol, sRTG_N_TFCR);
                    ckt_glb->RTG_test_pattern_report(sRTG_test_pattern_out);
                    ckt_glb->RTG_FC_report(sRTG_N_tol, sRTG_N_TFCR, sRTG_FC_out);
                    std::cout<<"End RTG" << std::endl;
                }
                std::cin.clear();
                std::cin.sync();
                break;

            case (11):  // PODEM cmd
                std::cin>>ckt_glb->PODEM_ssa_node;
                std::cin>>ckt_glb->PODEM_ssa_val;

                PODEM_iter_val = 0;

                if(!ckt_glb->read_done()){
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }else{
                    PODEM_start_time = clock();
                    ckt_glb->PODEM_reset();
                    if(ckt_glb->PODEM(1, PODEM_iter_val) == PODEM_result::PODEM_SUCCESS)
                    {
                        ckt_glb->sPODEM(ckt_fin);
                    }
                    else
                    {
                        std::cout<<"PODEM failed!"<<std::endl;
                    }
                    PODEM_end_time = clock();
                    // std::cout<<"End PODEM" << std::endl;
                    // std::cout<<"===== PODEM input value ====="<<std::endl;
                    // for(node* pi_node: ckt_glb->Pinput)
                    // {
                    //     std::cout<<pi_node->num <<"-"<<pi_node->logic_5_to_char(pi_node->PODEM_val_temp)<<std::endl;
                    // }
                    // std::cout<<"===== PODEM output value ====="<<std::endl;
                    // for(node* po_node: ckt_glb->Poutput)
                    // {
                    //     std::cout<<po_node->num <<"-"<<po_node->logic_5_to_char(po_node->PODEM_val_temp)<<std::endl;
                    // }
                    // std::cout<<"dump value finished"<<std::endl;
                    PODEM_run_time = (double)(PODEM_end_time - PODEM_start_time) / CLOCKS_PER_SEC;
                    // std::cout<<"PODEM run time is: "<<PODEM_run_time<<"sec"<<std::endl;
                    printf("PODEM run time is: %.4f sec\n", PODEM_run_time);
                }
                break;
            
            case (12):
                std::cin>>ckt_glb->DALG_ssa_node;
                std::cin>>ckt_glb->DALG_ssa_val;
                ckt_glb->set_DALG_timeout_coefficient();
                cout << "->DALG Timeout coefficient is set to: " << ckt_glb->DALG_TO_coeff << endl;
                if(!ckt_glb->read_done()){
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }
                else{
                    ckt_glb->DALG(ckt_glb->DALG_ssa_node, ckt_glb->DALG_ssa_val,ckt_fin,dpf,1);
                }
                break;

            case(13):   // ATPG_DET
                std::cin>>ckt_fin;
                std::cin>>ATPG_ALGO;
                ckt_glb = new circuit(ckt_fin);
                std::cout<<"initial passed!"<<std::endl;
                if(!ckt_glb->read_done()){
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }
                else
                {
                    transform(ATPG_ALGO.begin(), ATPG_ALGO.end(), ATPG_ALGO.begin(), ::tolower);
                    // if(ATPG_ALGO.compare(cmd_table[11]) == 0)    // PODEM
                    // {
                    //     PODEM_start_time = clock();
                    //     std::cout<<"get start time passed!"<<std::endl;
                    //     for(node* each_node: ckt_glb->Node)
                    //     {
                            
                    //         ckt_glb->PODEM_ssa_node = each_node->num;
                    //         ckt_glb->PODEM_ssa_val = 0;
                    //         ckt_glb->PODEM_reset();
                    //         std::cout<<"===== start "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                    //         PODEM_iter_val = 0;
                    //         if(ckt_glb->PODEM(1, PODEM_iter_val) == PODEM_result::PODEM_SUCCESS)
                    //         {
                    //             ckt_glb->PODEM_coverage_map_0[each_node->num] = 1;
                    //             ckt_glb->sPODEM(ckt_fin);
                    //         }
                    //         std::cout<<"===== -end- "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                    //         ckt_glb->PODEM_ssa_val = 1;
                    //         ckt_glb->PODEM_reset();
                    //         std::cout<<"===== start "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                    //         PODEM_iter_val = 0;
                    //         if(ckt_glb->PODEM(1, PODEM_iter_val) == PODEM_result::PODEM_SUCCESS)
                    //         {
                    //             ckt_glb->PODEM_coverage_map_1[each_node->num] = 1;
                    //             ckt_glb->sPODEM(ckt_fin);
                    //         }
                    //         std::cout<<"===== -end- "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                    //     }
                    //     PODEM_end_time = clock();
                    //     std::cout<<"get end time passed!"<<std::endl;
                    //     PODEM_run_time = (double)(PODEM_end_time - PODEM_start_time) / CLOCKS_PER_SEC;
                    //     ckt_glb->PODEM_save_report(PODEM_run_time, ckt_fin);
                    // }
                    if(ATPG_ALGO.compare(cmd_table[11]) == 0)    // PODEM
                    {
                        std::cout<<"----- PODEM START -----"<<std::endl;
                        ckt_glb->ATPG_PODEM_noRFL(ckt_fin);
                        std::cout<<"-----  PODEM END  -----"<<std::endl;
                    }
                    else if(ATPG_ALGO.compare(cmd_table[12]) == 0)
                    {
                        std::string DALG_pattern_fout = ckt_glb->get_cname(ckt_fin) + "_DALG_ATPG_pattern.txt";
                        dpf.open(DALG_pattern_fout,ios::out);

                        ckt_glb->set_DALG_timeout_coefficient();
                        cout << "->DALG Timeout coefficient is set to: " << ckt_glb->DALG_TO_coeff << endl;

                        const int PI_size = ckt_glb->Pinput.size();
                        for(int i=0;i<PI_size;i++){
                            if(i == PI_size-1){
                                dpf << ckt_glb->Pinput[i]->num << endl;
                            }
                            else
                            {
                                dpf << ckt_glb->Pinput[i]->num << ",";
                            }
                        }
                        
                        vector <int> DALG_uncover_0;
                        vector <int> DALG_uncover_1;
                        ckt_glb->DALG_coverage_map_0.clear();
                        ckt_glb->DALG_coverage_map_1.clear();
                        DALG_start_time = clock();
                        std::cout<<"get start time passed!"<<std::endl;
                        #ifdef DFS_REDUCTION_FOR_DALG
                        for(node* each_node: ckt_glb->Node)
                        {
                            ckt_glb->DALG_ssa_node = each_node->num;
                            ckt_glb->DALG_ssa_val = 0;
                            if(!ckt_glb->DALG_coverage_map_0[each_node->num]){
                                if(ckt_glb->DALG(ckt_glb->DALG_ssa_node, ckt_glb->DALG_ssa_val, ckt_fin, dpf,0) == true)
                                {
                                    ckt_glb->DALG_coverage_map_0[each_node->num] = 1;
                                }
                                else{
                                    DALG_uncover_0.push_back(each_node->num);
                                }
                            }

                            ckt_glb->DALG_ssa_val = 1;
                            if(!ckt_glb->DALG_coverage_map_1[each_node->num]){
                                if(ckt_glb->DALG(ckt_glb->DALG_ssa_node, ckt_glb->DALG_ssa_val, ckt_fin, dpf,0) == true)
                                {
                                    ckt_glb->DALG_coverage_map_1[each_node->num] = 1;
                                }
                                else{
                                    DALG_uncover_1.push_back(each_node->num);
                                }
                            }


                        #else
                            ckt_glb->DALG_ssa_node = each_node->num;
                            ckt_glb->DALG_ssa_val = 0;
                            // std::cout<<"===== start "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                            if(ckt_glb->DALG(ckt_glb->DALG_ssa_node, ckt_glb->DALG_ssa_val, ckt_fin) == true)
                            {
                                ckt_glb->DALG_coverage_map_0[each_node->num] = 1;
                            }
                            // std::cout<<"===== -end- "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                            ckt_glb->DALG_ssa_val = 1;
                            // ckt_glb->PODEM_reset();
                            // std::cout<<"===== start "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                            if(ckt_glb->DALG(ckt_glb->DALG_ssa_node, ckt_glb->DALG_ssa_val, ckt_fin) == true)
                            {
                                ckt_glb->DALG_coverage_map_1[each_node->num] = 1;
                            }
                            std::cout<<"===== -end- "<< ckt_glb->PODEM_ssa_node << "@"<< ckt_glb->PODEM_ssa_val<<std::endl;
                        #endif
                        }
                        dpf.close();
                        DALG_end_time = clock();
                        std::cout<<"get end time passed!"<<std::endl;
                        DALG_run_time = (double)(DALG_end_time - DALG_start_time) / CLOCKS_PER_SEC;
                        ckt_glb->DALG_save_report(DALG_run_time, ckt_fin);
                    }
                    else
                    {
                        std::cout<<"invalid command "<<ATPG_ALGO<<std::endl;
                    }
                    

                }
                std::cin.clear();
                std::cin.sync();	
                break;

            case(14):   // ATPG
                std::cin>>ckt_fin;
                ckt_glb = new circuit(ckt_fin);

                ckt_glb->set_DALG_timeout_coefficient();

                if(!ckt_glb->read_done()){
                    std::cout<<"==> circuit is not Read yet!"<<std::endl;
                    std::cout<<"==> please read the circuit first!"<<std::endl;
                }else {
                    std::cout << "start ATPG" << std::endl;
                    ckt_glb->ATPG_PODEM_2(ckt_fin);
                    std::cout << "end ATPG" << std::endl;
                }
                break;
                
            default:
                std::cout<< "==> " << cmd_name << " is not a legal cmd!" << std::endl; 
                std::cin.clear();
                std::cin.sync();	
                break;
        }
	}
    delete ckt_glb;
	return;
}

void TEST_MODE(string ckt_name, int num_test){
    string ckt_path = "./golden/"+ckt_name+".ckt";
    string lev_path = "./lev/"+ckt_name+"_lev.txt";

    // ckt->pc();
    // ckt->plev(ckt_name);
    for(int i=0; i<num_test; i++){
        circuit *ckt = new circuit(ckt_path);
        // ckt->lev(); //lev already in constructor, no longer needed here
        ckt->slev(lev_path,ckt_path);
        string logsim_test_path = "./golden/"+ckt_name+"_t"+to_string(i)+".txt";
        string logsim_output_path = "./self/SELF_"+ckt_name+"_t"+to_string(i)+"_output.txt";
        string logsim_5_test_path = "./"+ckt_name+"_t"+to_string(i)+".in.txt";
        //string logsim_5_test_path = "./"+ckt_name+"_t"+".in.txt";
        ckt->log_sim_5(logsim_5_test_path);
        ckt->plog(0);
        delete ckt;
    }
    
}

void BATCH_MODE(string batch_file_name)
{
    circuit *ckt_batch=NULL;
    
    std::string cmd_name;
    std::string ckt_fin;
    std::string test_fin;
    std::string slev_fout;
    std::string slog_fout;

    int return_idx;

    fstream batch_file;
    batch_file.open(batch_file_name, ios::in);
    string batch_cmd;

    while (getline(batch_file, batch_cmd))
	{
        stringstream cmd_stream(batch_cmd);
        cmd_stream>>cmd_name;
		return_idx = find_cmd(cmd_table, cmd_name);
//		std::cout<<return_idx<<"   " <<cmd_name <<std::endl;
		
        switch(return_idx)
        {  
            case(0):
                if (ckt_batch) delete ckt_batch;
                batch_file.close();
                return;

            case (1):       // read cmd
                cmd_stream>>ckt_fin;
                ckt_batch = new circuit(ckt_fin);
                break;

            case (2):       // print circuit cmd
                if(!ckt_batch->read_done())
                {
                    std::cout<<"==> circuit is not read yet!"<<std::endl;
                    std::cout<<"==>please read a circuit first!"<<std::endl;
                }
                else
                {
                    ckt_batch->pc();
                }
                break;

            case(3):        // lev cmd
                cmd_stream>>slev_fout;
                if(!ckt_batch->read_done())
                {
                    std::cout<<"==> circuit is not read yet!"<<std::endl;
                    std::cout<<"==> please read a circuit first!"<<std::endl;
                }
                else
                {
                    ckt_batch->lev();
                    ckt_batch->slev(slev_fout,ckt_fin);
                }
                break;
            
            case(4):        // plev cmd
                if(!ckt_batch->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
                    ckt_batch->plev(ckt_fin);
                }
                break;

            case(5):        //log_sim cmd
                cmd_stream>>test_fin;
                cmd_stream>>slog_fout;
                // std::cout<< "test_fin is '" << test_fin <<"' and slog_fout is '"<< slog_fout << "'" << std::endl;
                if(!ckt_batch->lev_done())
                {
                    std::cout<<"==> circuit is not levelized yet!"<<std::endl;
                    std::cout<<"==> please levelize the circuit first!"<<std::endl;
                }
                else
                {
                	// std::cout<<"start log_sim" << std::endl;
                    ckt_batch->log_sim(test_fin);
                    // std::cout<<"start slog" << std::endl;
                    ckt_batch->slog(slog_fout);
                    // std::cout<<"end slog" << std::endl;
                }
                break;

            case(6):        // plog cmd
                if(!ckt_batch->sim_done())
                {
                    std::cout<<"==> circuit is not simulated yet!"<<std::endl;
                    std::cout<<"==> please simulate the circuit first!"<<std::endl;
                }
                else
                {
                    ckt_batch->plog();
                }
                break;

            default:
                std::cout<< "==> " << cmd_name << " is not a legal cmd!" << std::endl; 
                break;
        }
        
	}
    delete ckt_batch;
    batch_file.close();
	return;
}
