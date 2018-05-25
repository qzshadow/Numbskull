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
    W_Send_D, W_Send_P_D, W_Send_P_E, W_Send_P_G,
};

int main() {
    mpi::environment env;
    mpi::communicator world;
    const int master_rank = 0;
    const std::vector<int> workers_rank = {1, 2};
    size_t num_samples = 10000;

    std::unordered_map<size_t, std::array<int, 2>> counter;
    FactorGraph graph;

    if (world.rank() == 0) {
        auto* var0 = new BinaryVariable(0, 0, 0.0, "A");
        auto* edge0 = new IdentityEdge(0, var0, "A");
        auto* edge2 = new IdentityEdge(2, var0, "A");
        auto* pfactor0 = new PatialAndFactor(0, {edge0}, 1.0, "G1");
        auto* pfactor2 = new PatialAndFactor(2, {edge2}, 1.0, "G2");
        graph.var_ptr_map["A"] = {var0};
        graph.partial_factor_ptr_map["G1"] = {pfactor0};
        graph.partial_factor_ptr_map["G2"] = {pfactor2};
        var0->set_factor_vec({pfactor0, pfactor2});
    } else if (world.rank() == 1) {
        auto* var1 = new BinaryVariable(1, 0, 0.0, "C");
        auto* edge1 = new IdentityEdge(1, var1, "C");
        auto* pfactor1 = new PatialAndFactor(1, {edge1}, 1.0, "G");
        graph.var_ptr_map["C"] = {var1};
        graph.partial_factor_ptr_map["G"] = {pfactor1};
        var1->set_factor_vec({pfactor1});
    } else if (world.rank() == 2) {
        auto* var2 = new BinaryVariable(2, 0, 0.0, "C");
        auto* edge3 = new IdentityEdge(3, var2, "C");
        auto* pfactor3 = new PatialAndFactor(3, {edge3}, 1.0, "G");
        graph.var_ptr_map["C"] = {var2};
        graph.partial_factor_ptr_map["G"] = {pfactor3};
        var2->set_factor_vec({pfactor3});
    }

    while (num_samples--) {
        if (world.rank() == master_rank) { // master
            // master computes partial evaluation of all G_i-key factors, and transmits them to worker i
            for (auto worker_rank : workers_rank) {
                std::string key = "G" + std::to_string(worker_rank);
                std::vector<float> send_partial_factor_vec(graph.partial_factor_ptr_map[key].size());
                for (size_t i = 0; i < send_partial_factor_vec.size(); ++i)
                    send_partial_factor_vec[i] = graph.partial_factor_ptr_map[key][i]->partial_eval({"A"});
                world.send(worker_rank, static_cast<int>(MsgType::M_Send_P_G), send_partial_factor_vec);
            }
            // master receive G partial factor from workers, update it's own cache
            for (auto worker_rank : workers_rank) {
                std::string key = "G" + std::to_string(worker_rank);
                std::vector<float> receive_partial_factor_vec;
                world.recv(worker_rank, static_cast<int>(MsgType::W_Send_P_G), receive_partial_factor_vec);
                for (size_t i = 0; i < receive_partial_factor_vec.size(); ++i)
                    graph.partial_factor_ptr_map[key][i]->set_partial_val(receive_partial_factor_vec[i]);
            }
            // master runs a single pass of Gibbs sampling on the variable it owns
            for (auto& var : graph.var_ptr_map["A"]) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }

        } else { // workers
            // worker receive all transmission from master, updates its G partial factor
            std::vector<float> receive_partial_factor_vec;
            world.recv(master_rank, static_cast<int>(MsgType::M_Send_P_G), receive_partial_factor_vec);
            for (size_t i = 0; i < receive_partial_factor_vec.size(); ++i)
                graph.partial_factor_ptr_map["G"][i]->set_partial_val(receive_partial_factor_vec[i]);
            // worker runs a single pass Gibbs sampling on the variable it owns
            for (auto& var : graph.var_ptr_map["C"]) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }
            // worker computes partial evaluation of all G_i key factors, transmit them to master
            std::vector<float> send_partial_factor_vec(graph.partial_factor_ptr_map["G"].size());
            for (size_t i = 0; i < send_partial_factor_vec.size(); ++i)
                send_partial_factor_vec[i] = graph.partial_factor_ptr_map["G"][i]->partial_eval({"C"});
            world.send(master_rank, static_cast<int>(MsgType::W_Send_P_G), send_partial_factor_vec);


        }
    }
    for (auto& entry : counter) {
        for (int v = 0; v < 2; ++v) {
            std::cout << "var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
        }
    }


    return 0;
}
