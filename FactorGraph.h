/*
 * FactorGraph class
 * This class is used to construct graph prototype
 * Contains information of the whole graph manifested on a machine
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
/*
 * MsgType describe the protocol message between master and workers
 */
enum class MsgType {
    M_Send_B, // Master send B type variable values to workers
    M_Send_P_E, // Master send partial evaluation on E type factors to workers
    M_Send_P_G, // Master send partial evaluation on G type factors to workers
    W_Send_D, // Workers send D type variable values to master
    W_Send_P_D, // Workers send partial evaluation on D type factors to master
    W_Send_P_G, // workers send partial evaluation on G type factors to master
};

class FactorGraph {
public:
    // Generators for different types of graph (for testing the framework)
    void gen_BDC_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor);
    void gen_AGC_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor);
    void gen_AED_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor);
    void gen_BFD_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor);

    // perform gibbs sampling on current graph
    void gibbs(size_t num_samples, int master_rank, std::vector<int> workers_rank);

public:
    /*
     * Information of this graph
     * format map<assign, vector>
     */
    std::unordered_map<std::string, std::vector<Factor *>> factor_ptr_map;
    std::unordered_map<std::string, std::vector<PartialFactor *>> partial_factor_ptr_map;
    std::unordered_map<std::string, std::vector<Variable *>> var_ptr_map;

    // mpi::environment env;
    // world contains the cluster information
    // e.g. world.rank() is the ordinary of machine
    mpi::communicator world;

    // clean the memory when class deconstructed
    ~FactorGraph() {
        for (auto& entry : factor_ptr_map) {
            for (Factor* ptr : entry.second) delete ptr;
        }

        for (auto& entry : partial_factor_ptr_map) {
            for (PartialFactor* ptr : entry.second) delete ptr;
        }

        for (auto& entry : var_ptr_map) {
            for (Variable * ptr : entry.second) delete ptr;
        }
    }

};


#endif //NUMBSKULL_FACTORGRAPH_H
