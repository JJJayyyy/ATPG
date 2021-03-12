//#include "circuit.hpp"

void circuit:: read(string file_name){
    node *np;

    string line;

    Nnodes = Npi = Npo = NGates = 0;

    fin.open(file_name,ios::in);

    while(getline(fin,line)){
        if(!isdigit(line[0])) continue;
        if(sscanf(line.c_str(),"%d %d", &tp, &nd) == 2){
            if(ntbl < nd) ntbl = nd;
            Nnodes ++;
            if(tp == static_cast<int>(e_ntype::PI)) Npi++;
            else if(tp == static_cast<int>(e_ntype::PO)) Npo++;
        }
    }

    vector<int> tbl(ntbl+1);
    fin.clear();
    fin.seekg(0);

    int i = 0;

    while(getline(fin,line)){
        if(!isdigit(line[0])) continue;
        if(sscanf(line.c_str(),"%d %d", &tp, &nd) == 2) tbl[nd] = i++;
    }
    fin.clear();
    fin.seekg(0);

    for(int i = 0; i<Nnodes; i++){
        Node.push_back(new node()); // construct all the node* in Node array here
        Node[i]->idx = i;
        Node[i]->fin = Node[i]->fout = 0;
    }


    while(getline(fin,line)){
    
        if(!isdigit(line[0])){
            continue;
        }

        int e_type;
        stringstream col(line);
        col >> tp;
        col >> nd;

        np = Node[tbl[nd]];
        np->num = nd;
    
        if(tp == static_cast<int>(e_ntype::PI)){
            Pinput.push_back(np);
            np->node_type = e_ntype::PI;
        }
           
        else if(tp == static_cast<int>(e_ntype::PO)) {
            Poutput.push_back(np);
            np->node_type = e_ntype::PO;
        }
        switch(tp) {
            case static_cast<int>(e_ntype::PI):
            case static_cast<int>(e_ntype::PO):
            case static_cast<int>(e_ntype::GATE):
                col >> e_type;
                col >> np->fout;
                col >> np->fin;
                if(2<=e_type && e_type<=7){
                    NGates++;
                }
                np->node_type = e_ntype::GATE;
                np->type = static_cast<e_gtype> (e_type);
                np->type_num = e_type;
                break;
            
            case static_cast<int>(e_ntype::FB): 
                np->fout = 1;
                np->fin = 1;
                col >> e_type;
                np->node_type = e_ntype::FB;
                np->type = static_cast<e_gtype> (e_type);
                np->type_num = e_type;
                break;

            default:
                printf("Unknown node type!\n");
                exit(-1);
        }

        for(i = 0; i < np->fin; i++) { // according to fin(col5) allocate the unodes(upstream nodes) array of this np(node)
            col >> nd;
            np->unodes.push_back(Node[tbl[nd]]);
        }

    }

    for(int i = 0; i < Nnodes; i++) {
        for(int j = 0; j < Node[i]->fin; j++) {
            np = Node[i]->unodes[j]; // upstream node of Node[i]=np
            
            if(np->dnodes.size() < np->fout){
                np->dnodes.push_back(Node[i]);
            } 

        }
    }
fin.close();

}




