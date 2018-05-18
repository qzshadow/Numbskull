//
// Created by zq32 on 5/7/18.
//

#include "FactorGraph.h"
#include <boost/mpi/timer.hpp>


int main(int argc, char *argv[]) {
    // mpirun -np 3 Numbskull graph_type num_samples num_var_on_master, num_fac_on_worker, num_var_per_fac
    //const std::vector<int> workers_rank = {1, 2}
    assert(argc == 6);
    std::string gtype = argv[1];
    size_t num_samples = atoll(argv[2]);
    size_t var_num_on_master = atoll(argv[3]);
    size_t factor_num_per_worker = atoll(argv[4]);
    size_t var_num_per_factor = atoll(argv[5]);


    const int master_rank = 0;
    FactorGraph graph;
    std::vector<int> workers_rank;
    for (int i = 1; i < graph.world.size(); ++i) workers_rank.push_back(i);

    // graph.gen_BDC_instance(workers_rank.size(),2,1,1);
    // graph.gen_AGC_instance(workers_rank.size(), 2, 1,1);
    // graph.gen_AED_instance(workers_rank.size(), 1,1,2);


    if (gtype == "BDC") graph.gen_BDC_instance(workers_rank.size(), var_num_on_master, factor_num_per_worker, var_num_per_factor);
    else if (gtype == "BFD") graph.gen_BFD_instance(workers_rank.size(), var_num_on_master, factor_num_per_worker, var_num_per_factor);
    else if (gtype == "AGC") graph.gen_AGC_instance(workers_rank.size(), var_num_on_master, factor_num_per_worker, var_num_per_factor);
    else if (gtype == "AED") graph.gen_AED_instance(workers_rank.size(), var_num_on_master, factor_num_per_worker, var_num_per_factor);
    else std::cerr<<"unsupport graph type";

    // auto timer = mpi::timer();
    graph.gibbs(num_samples, master_rank, workers_rank);
//    if (graph.world.rank() == 0) {
//        std::cout<<gtype<<"\t"<<num_samples<<"\t"<<var_num_on_master<<"\t"<<factor_num_per_worker<<"\t"<<var_num_per_factor<<std::endl;
//        std::cout<<timer.elapsed()<<std::endl;
//    }
}
