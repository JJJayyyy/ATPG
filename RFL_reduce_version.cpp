//
// Created by Jay on 10/20/20.
//
#include "circuit.hpp"
void circuit::RFL_reduce(){
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

    // when rfl_x is 0, we would like to drop this from to_check list; 1, wouldn't
    for (int i = 0; i < Nnodes; i++){
        np = Node[i];
        if (np->node_type==e_ntype::PI){  // find PI
            if (np -> rfl_0 == 1 && np -> rfl_1 == 1){ // if all the fault of PI was not dropped
                switch(np->dnodes[0]->type){  // find the it's equivalence
                    case(e_gtype::OR): {
                        bool drop = false;
                        for(int j=0; j<np->dnodes[0]->unodes.size(); j++){ // find all the inputs of the next gate
                            if(2<=np->dnodes[0]->unodes[j]->type_num && np->dnodes[0]->unodes[j]->type_num<=7){
                                // Ignore this input, if it is gate type,
                                continue;
                            } else{
                                if(np->dnodes[0]->unodes[j]!=np){ // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[j]->rfl_1 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if(!drop){
                            np->rfl_1 = 0;
                        }
                    }
                        break;
                    case(e_gtype::NOR): {
                        bool drop = false;
                        for(int j=0; j<np->dnodes[0]->unodes.size(); j++){ // find all the inputs of the next gate
                            if(2<=np->dnodes[0]->unodes[j]->type_num && np->dnodes[0]->unodes[j]->type_num<=7){
                                // Ignore this input, if it is gate type,
                                continue;
                            } else{
                                if(np->dnodes[0]->unodes[j]!=np){ // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[j]->rfl_1 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if(!drop){
                            np->rfl_1 = 0;
                        }
                    }
                        break;
                    case(e_gtype::AND): {
                        bool drop = false;
                        for(int j=0; j<np->dnodes[0]->unodes.size(); j++){ // find all the inputs of the next gate
                            if(2<=np->dnodes[0]->unodes[j]->type_num && np->dnodes[0]->unodes[j]->type_num<=7){
                                // Ignore this input, if it is gate type,
                                continue;
                            } else{
                                if(np->dnodes[0]->unodes[j]!=np){ // if this input was not the node, drop its s@0
                                    np->dnodes[0]->unodes[j]->rfl_0 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if(!drop){
                            np->rfl_0 = 0;
                        }
                    }
                        break;
                    case(e_gtype::NAND): {
                        bool drop = false;
                        for(int j=0; j<np->dnodes[0]->unodes.size(); j++){ // find all the inputs of the next gate
                            if(2<=np->dnodes[0]->unodes[j]->type_num && np->dnodes[0]->unodes[j]->type_num<=7){
                                // Ignore this input, if it is gate type,
                                continue;
                            } else{
                                if(np->dnodes[0]->unodes[j]!=np){ // if this input was not the node, drop its s@0
                                    np->dnodes[0]->unodes[j]->rfl_0 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if(!drop){
                            np->rfl_0 = 0;
                        }
                    }
                        break;
                    case (e_gtype::NOT):
                        break;
                    case (e_gtype::XOR):
                        break;
                    case (e_gtype::BRCH):
                        break;
                    case (e_gtype::IPT):
                        break;
                    default:
                        break;
                }
            } else{
                continue;
            }
        } else if (np->type==e_gtype::BRCH){
            // if all the faults were not dropped
            if (np->rfl_0 == 1 && np->rfl_1 == 1) {
                switch (np->dnodes[0]->type) {  // find the it's equivalence
                    case (e_gtype::OR): {
                        bool drop = false;
                        for (int k = 0; k < np->dnodes[0]->unodes.size(); k++){
                            // find all the inputs of the next gate
                            if (2 <= np->dnodes[0]->unodes[k]->type_num &&
                                np->dnodes[0]->unodes[k]->type_num <= 7) {
                                // Ignore this input, if it is gate type,
                                continue;
                            } else {
                                if (np->dnodes[0]->unodes[k] != np) {
                                    // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[k]->rfl_1 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if (!drop) {
                            np->rfl_1 = 0;
                        }
                    }
                        break;
                    case (e_gtype::NOR): {
                        bool drop = false;
                        for (int k = 0; k < np->dnodes[0]->unodes.size(); k++) {
                            // find all the inputs of the next gate
                            if (2 <= np->dnodes[0]->unodes[k]->type_num &&
                                np->dnodes[0]->unodes[k]->type_num <= 7) {
                                // Ignore this input, if it is gate type,
                                continue;
                            } else {
                                if (np->dnodes[0]->unodes[k] != np) {
                                    // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[k]->rfl_1 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if (!drop) {
                            np->rfl_1 = 0;
                        }
                    }
                        break;
                    case (e_gtype::AND): {
                        bool drop = false;
                        for (int k = 0; k < np->dnodes[0]->unodes.size(); k++) {
                            // find all the inputs of the next gate
                            if (2 <= np->dnodes[0]->unodes[k]->type_num &&
                                np->dnodes[0]->unodes[k]->type_num <= 7) {
                                // Ignore this input, if it is gate type,
                                continue;
                            } else {
                                if (np->dnodes[0]->unodes[k] != np) {
                                    // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[k]->rfl_0 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if (!drop) {
                            np->rfl_0 = 0;
                        }
                    }
                    break;
                    case (e_gtype::NAND): {
                        bool drop = false;
                        for (int k = 0; k < np->dnodes[0]->unodes.size(); k++) {
                            // find all the inputs of the next gate
                            if (2 <= np->dnodes[0]->unodes[k]->type_num &&
                            np->dnodes[0]->unodes[k]->type_num <= 7) {
                                // Ignore this input, if it is gate type,
                                continue;
                            } else {
                                if (np->dnodes[0]->unodes[k] != np) {
                                    // if this input was not the node, drop its s@1
                                    np->dnodes[0]->unodes[k]->rfl_0 = 0;
                                    drop = true;
                                }
                            }
                        }
                        if (!drop) {
                            np->rfl_0 = 0;
                        }
                    }
                    break;
                    case (e_gtype::NOT):
                        break;
                    case (e_gtype::XOR):
                        break;
                    case (e_gtype::BRCH):
                        break;
                    case (e_gtype::IPT):
                        break;
                    default:
                        break;
                    }
                } else {
                    continue;
                }
        } else{
            continue;
        }
    }

//    cout << "--------------RFL info-----------------" << endl;
//    for (int i = 0; i < Nnodes; i++){
//        np = Node[i];
//        if(np->type == e_gtype::IPT || np->type == e_gtype::BRCH){  // find PI and FB
//            if(np->rfl_1 == 1){
//                cout << "Node"<< np->num << "@1" << endl;
//            }
//            if (np->rfl_0 == 1){
//                cout << "Node"<< np->num << "@0" << endl;
//            }
//        }
//    }
}