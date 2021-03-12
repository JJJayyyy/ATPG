//
// Created by Jay on 10/28/20.
//

#include "circuit.hpp"
void circuit::RFL(string output_file_name){
    if(this->read_flag == 0)
    {
        std::cout<<"The circuit should be done before RFL!"<<std::endl;
        return;
    }
    // initialize rfl fault of PI and FB to 1, other's to -1
    for (int i = 0; i < Nnodes; i++){
        np = Node[i];
        np -> rfl_0 = -1;
        np -> rfl_1 = -1;
        if(np->type == e_gtype::IPT || np->type == e_gtype::BRCH)
        {
            np -> rfl_0 = 1;
            np -> rfl_1 = 1;
        }
    }

    fp.open(output_file_name, ios::out);
    for (int i = 0; i < Nnodes; i++){ // output RFL result to the file
        np = Node[i];
        if(np->type == e_gtype::IPT || np->type == e_gtype::BRCH){  // find PI and FB
            if(np->rfl_1 == 1){
                fp << np->num << "@1" << endl;
            }
            if (np->rfl_0 == 1){
                fp << np->num << "@0" << endl;
            }
        }
    }
    cout << "--------------RFL finished-----------------" << endl;
    fp.close();
}