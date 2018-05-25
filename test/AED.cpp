//
// Created by zq32 on 4/16/18.
//
#include <iostream>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include "../FactorGraph.h"

namespace mpi = boost::mpi;
enum class MsgType {
    M_Send_B, M_Send_P_E, M_Send_P_G,
    W_Send_D, W_Send_P_D, W_Send_P_E,
};
int main() {
    mpi::environment env;
    mpi::communicator world;
    const int master_rank = 0;
    const std::vector<int> workers_rank = {1, 2};
    size_t num_samples = 10000;

    std::unordered_map<size_t, std::array<int, 2>> counter;
    FactorGraph graph;

    // construct the graph for each machine
    if (world.rank() == 0) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "A");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "A");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D1");
        auto *var3 = new BinaryVariable(3, 0, 0.0, "D2");
        auto *edge0 = new IdentityEdge(0, var0, "A");
        auto *edge1 = new IdentityEdge(1, var1, "A");
        auto *edge2 = new IdentityEdge(1, var2, "A");
        auto* edge3 = new IdentityEdge(3, var0, "A");
        auto* edge4 = new IdentityEdge(4, var1, "A");
        auto* edge5 = new IdentityEdge(5, var3, "A");
        auto *factor0 = new AndFactor(0, {edge0, edge1, edge2}, 1.0, "E1");
        auto *factor1 = new AndFactor(1, {edge3, edge4, edge5}, 1.0, "E2");
        var0->set_factor_vec({factor0, factor1});
        var1->set_factor_vec({factor0, factor1});
        graph.var_ptr_map["A"] = {var0, var1};
        graph.var_ptr_map["D1"] = {var2};
        graph.var_ptr_map["D2"] = {var3};
        graph.factor_ptr_map["E1"] = {factor0};
        graph.factor_ptr_map["E2"] = {factor1};

    } else if (world.rank() == 1) {
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D");
        auto *edge2 = new IdentityEdge(2, var2, "D");
        auto *pfactor0 = new PatialAndFactor(0, {edge2}, 1.0, "E");
        graph.var_ptr_map["D"] = {var2};
        graph.partial_factor_ptr_map["E"] = {pfactor0};
        var2->set_factor_vec({pfactor0});
    } else if (world.rank() == 2) {
        auto *var3 = new BinaryVariable(3, 0, 0.0, "D");
        auto *edge5 = new IdentityEdge(5, var3, "D");
        auto *pfactor1 = new PatialAndFactor(1, {edge5}, 1.0, "E");
        graph.var_ptr_map["D"] = {var3};
        graph.partial_factor_ptr_map["E"] = {pfactor1};
        var3->set_factor_vec({pfactor1});
    }

    // sampling begins
    while (num_samples--) {
        // TODO check if graph has be initialized
        if (world.rank() == master_rank) { // master machine
            // master computes partial evaluation of all E_i-key factors, transmits them to worker i
            for (auto worker_rank : workers_rank) {
                std::string key = "E" + std::to_string(worker_rank);
                std::vector<float> partial_factor_vec(graph.factor_ptr_map[key].size());
                for (int i = 0; i < partial_factor_vec.size(); ++i)
                    partial_factor_vec[i] = graph.factor_ptr_map[key][i]->partial_eval({"A"});
                world.send(worker_rank, static_cast<int>(MsgType::M_Send_P_E), partial_factor_vec);
            }
            // master receive D variable values from workers
            for (auto worker_rank : workers_rank) {
                std::string key = "D" + std::to_string(worker_rank);
                std::vector<int> var_vec;
                world.recv(worker_rank, static_cast<int>(MsgType::W_Send_D), var_vec);
                for (size_t i = 0; i < var_vec.size(); ++i)
                    graph.var_ptr_map[key][i]->set_value(var_vec[i]);
            }
            // master run a single pass of gibbs sampling on the variables it owns
            for (auto& var : graph.var_ptr_map["A"]) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }


        } else { // workers
            // receive partial factor values from master
            std::vector<float> partial_factor_vec;
            world.recv(master_rank, static_cast<int>(MsgType::M_Send_P_E), partial_factor_vec);
            // update E partial factor
            for (size_t i = 0; i < partial_factor_vec.size(); ++i)
                graph.partial_factor_ptr_map["E"][i]->set_partial_val(partial_factor_vec[i]);
            // runs a single pass of Gibbs sampling on the variable it owns
            for (auto& var : graph.var_ptr_map["D"]) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }
            // transmit D-key variables to master
            std::vector<int> var_vec(graph.var_ptr_map["D"].size());
            for (size_t i = 0; i < var_vec.size(); ++i)
                var_vec[i] = graph.var_ptr_map["D"][i]->get_value();
            world.send(master_rank, static_cast<int>(MsgType::W_Send_D), var_vec);
        }
    }
    for (auto& entry : counter) {
        for (int v = 0; v < 2; ++v) {
            std::cout << "var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
        }
    }

    return 0;
}
