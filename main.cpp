//
// Created by zq32 on 4/3/18.
//
#include <iostream>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include "FactorGraph.h"

namespace mpi = boost::mpi;
enum class MsgType {
    M_Send_B, W_Send_P_Fac
};

int main() {
    mpi::environment env;
    mpi::communicator world;
    const int master_rank = 0;
    const std::vector<int> workers_rank = {1, 2};
    size_t num_samples = 10000;

    std::unordered_map<size_t, std::array<int, 2>> counter;
    FactorGraph graph;
    // construct the graph for three machine
    if (world.rank() == 0) { // master
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *edge0 = new IdentityEdge(0, var0, "D1");
        auto *edge2 = new IdentityEdge(2, var0, "D2");
        graph.owned_var_ptr_vec["B"] = {var0};
        auto *partial_fac0 = new PatialAndFactor(0, {edge0}, 1.0, "D1");
        auto *partial_fac1 = new PatialAndFactor(1, {edge2}, 1.0, "D2");
        graph.partial_factor_ptr_vec["D1"] = {partial_fac0};
        graph.partial_factor_ptr_vec["D2"] = {partial_fac1};
        var0->set_factor_vec({partial_fac0, partial_fac1});
    } else if (world.rank() == 1) { // worker0
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "C");
        auto *edge0 = new IdentityEdge(0, var0, "D");
        auto *edge1 = new IdentityEdge(1, var1, "C");
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0, "D");
        var1->set_factor_vec({factor0});
        graph.owned_var_ptr_vec["C"] = {var1};
        graph.cached_var_ptr_vec["B"] = {var0};
        graph.owned_factor_ptr_vec["D"] = {factor0};

    } else if (world.rank() == 2) {// worker1
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "C");
        auto *edge2 = new IdentityEdge(2, var0, "D");
        auto *edge3 = new IdentityEdge(3, var2, "C");
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0, "D");
        var2->set_factor_vec({factor1});
        graph.owned_var_ptr_vec["C"] = {var2};
        graph.cached_var_ptr_vec["B"] = {var0};
        graph.owned_factor_ptr_vec["D"] = {factor1};

    }

    while (num_samples--) {
        if (world.rank() == master_rank) { // master
            // for B variable values vector, then send B variables values to workers
            std::vector<int> val_vec(graph.owned_var_ptr_vec["B"].size());
            for (size_t i = 0; i < val_vec.size(); ++i) val_vec[i] = graph.owned_var_ptr_vec["B"][i]->get_value();
            for (auto worker_rank : workers_rank)
                world.send(worker_rank, static_cast<int>(MsgType::M_Send_B), val_vec);
            // Master receive partial factors from worker
            for (auto worker_rank : workers_rank) {
                std::vector<float> partial_factor_vec = {};
                world.recv(worker_rank, static_cast<int>(MsgType::W_Send_P_Fac), partial_factor_vec);
                for (size_t i = 0; i < partial_factor_vec.size(); ++i) {
                    // TODO careful with the corresponding relationships
                    std::string key = "D" + std::to_string(worker_rank);
                    graph.partial_factor_ptr_vec[key][i]->set_partial_val(partial_factor_vec[i]);
                }
            }
            // Master perform a resample for all the variables it owns
            for (auto& var_ptr : graph.owned_var_ptr_vec["B"]) {
                var_ptr->resample();
                counter[var_ptr->get_vid()][var_ptr->get_value()]++;
            }

        } else { // for worker machines
            // recieve B variables transferred from master, save it as cached variable
            std::vector<int> val_vec(graph.cached_var_ptr_vec.size());
            world.recv(master_rank, static_cast<int>(MsgType::M_Send_B), val_vec);
            for (size_t i = 0; i < val_vec.size(); ++i) graph.cached_var_ptr_vec["B"][i]->set_value(val_vec[i]);
            // workers perform a sample for the variable they owned
            for (auto& var_ptr : graph.owned_var_ptr_vec["C"]) {
                var_ptr->resample();
                counter[var_ptr->get_vid()][var_ptr->get_value()]++;
            }
            // workers perform partial eval on the factor they owned

            std::vector<float> partial_factor_vec(graph.owned_factor_ptr_vec["D"].size());
            for (size_t i = 0; i < partial_factor_vec.size(); ++i) {
                partial_factor_vec[i] = graph.owned_factor_ptr_vec["D"][i]->partial_eval();
            }

            // then transform the partial (reduced) factors to master
            world.send(master_rank, static_cast<int>(MsgType::W_Send_P_Fac), partial_factor_vec);
        }
    }

    for (auto& entry : counter) {
        for (int v = 0; v < 2; ++v) {
            std::cout << "var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
        }
    }
    return 0;
}
