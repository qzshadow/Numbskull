/*
 * FactorGraph class
 * This class is used to generate the graph from input file
 * Contains information of the whole graph
 * and how to assign the graph to different machines
 */
#ifndef NUMBSKULL_FACTORGRAPH_H
#define NUMBSKULL_FACTORGRAPH_H

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <memory>
#include "Variable.h"
#include "Edge.h"
#include "Factor.h"
#include <vector>
#include <map>
#include <unordered_map>


namespace mpi = boost::mpi;
enum class MsgType {
    M_Send_B, M_Send_P_E, M_Send_P_G,
    W_Send_D, W_Send_P_D, W_Send_P_E, W_Send_P_G,
};

class FactorGraph {
public:
    void generate_BDC_instance();
    void generate_AGC_instance();
    void generate_AED_instance();
    void generate_BFD_instance();

    void resample(size_t num_samples, int master_rank, std::vector<int> workers_rank);
public:
    std::unordered_map<std::string, std::vector<Factor *>> factor_ptr_map;
    //std::unordered_map<std::string, std::vector<Factor *>> cached_factor_ptr_map;
    std::unordered_map<std::string, std::vector<PatialFactor *>> partial_factor_ptr_map;

    std::unordered_map<std::string, std::vector<Variable *>> var_ptr_map;
    //std::unordered_map<std::string, std::vector<Variable *>> cached_var_ptr_map;

    mpi::environment env;
    mpi::communicator world;


};


#endif //NUMBSKULL_FACTORGRAPH_H
