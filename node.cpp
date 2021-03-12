#include "node.hpp"

using namespace std;


static string get_gname(int tp){
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

// node:: ~node(){
//     for(int i = 0; i < unodes.size(); i++){
//         delete unodes[i];
//     }
//     for(int i = 0; i < dnodes.size(); i++){
//         delete dnodes[i];
//     }
//     cout << "unode & dnode destructed  \n";
// }

void node:: pn(){
    cout << "------Node information------\n";
    cout << "Node idx:\t" << idx << "\n";
    cout << "Node num:\t" << num << "\n";
    cout << "Node type:\t" << get_gname(static_cast<int>(type)) << "\n";
    cout << "Node fin:\t" << fin << "\n";
    cout << "Node fout:\t" << fout << "\n";
    cout << "\n";
    cout << "value_5:\t" << logic_5_to_char(value_5) << endl;
    for(int i = 0; i < unodes.size(); i++){
        if(unodes[i]!=NULL){
            cout << "unodes:\t" << unodes[i]->num << "-" << this->logic_5_to_char(unodes[i]->value_5) << "\n";
        }
    }
    for(int i = 0; i < dnodes.size(); i++){
        if(dnodes[i]!=NULL){
            cout << "dnodes:\t" << dnodes[i]->num << "-" << this->logic_5_to_char(dnodes[i]->value_5) << "\n";
        }
    }
    cout << "---------END---------\n";
}

char node:: logic_5_to_char(uint8_t logic_5_in){
    char char_out;
    switch (logic_5_in)
    {
    case (LOGIC_0):
        char_out = '0';
        break;
    case (LOGIC_1):
        char_out = '1';
        break;
    case (LOGIC_X):
    case (LOGIC_XX):
        char_out = 'X';
        break;
    case (LOGIC_D):
        char_out = 'D';
        break;
    case (LOGIC_Dn):
        char_out = 'N';
        break;
    default:
        cout << "logic_5_in: " << to_string(logic_5_in) << " is not a valid 'logic_5_typedef' type" << endl;
    }
    return char_out;
}


