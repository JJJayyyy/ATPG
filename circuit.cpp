#include "circuit.hpp"
#include "node.cpp"
#include "cread.cpp"
#include "clev.cpp"
#include "clog_sim.cpp"
//#include "clog_tmp.cpp"
#include "io.cpp"
#include "DFS_simulator.cpp"
#include "PFS_simulator.cpp"
#include "RFL_reduce_version.cpp"
#include "RFL.cpp"
#include "RTG.cpp"
#include "PODEM.cpp"
#include "DALG.cpp"
#include "ATPG.cpp"
#include "ATPG_RTG.cpp"

circuit::circuit(string ckt_file){
    read(ckt_file);
    this->read_flag = 1;
    this->lev(); // add levelization to constructor
}


int circuit::read_done(void)
{
    return this->read_flag;
}

int circuit::lev_done(void)
{
    return this->levelization_flag;
}

int circuit::sim_done(void)
{
    return this->simulation_flag;
}

