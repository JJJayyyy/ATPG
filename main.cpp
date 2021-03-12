#include "mode.hpp"
#include "mode.cpp"


int main(int argc, char *argv[]){

//    vector<string> ckt_name_vec{"c17","add2","c499","c880"};
//    vector<int> num_test_vec{6,6,3,1};

//    vector<string> ckt_name_vec{"c17"};
//    vector<int> num_test_vec{2};

//    for(int i=0; i<ckt_name_vec.size();i++){
//        TEST_MODE(ckt_name_vec[i],num_test_vec[i]);
//    }
	
    // if(argc != 1)
    // {
    //     for(int i = 1; i <= argc - 1; i++)
    //     {
    //         string batch_name = argv[i];
    //         BATCH_MODE(batch_name);
    //     }
    // }
    // else{
    //     CMD_MODE();
    // }

    CMD_MODE();

    // string ckt_path = "c17_t0.in.txt";
    // string ckt_name = "c17";
    // circuit *ckt = new circuit(ckt_path);
    // string logsim_5_test_path = "./"+ckt_name+"_t0"+".in.txt";
    // // ckt->log_sim(logsim_test_path);
    // // ckt->slog(logsim_output_path);
    // ckt->log_sim_5(logsim_5_test_path);
    // ckt->plog(0);
    // delete ckt;

}



