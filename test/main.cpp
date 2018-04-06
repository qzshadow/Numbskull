#include "../FactorGraph.h"
#include <iostream>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace mpi = boost::mpi;

enum class MsgType {
    M_Send_B, W_Send_D
};

int main() {
    mpi::environment env;
    mpi::communicator world;
    size_t num_samples = 1000;
    const int master_rank = 0;
    const std::vector<int> workers_rank = {1, 2};
    std::unordered_map<size_t, std::array<int, 2>> counter;

    FactorGraph graph;
    if (world.rank() == master_rank) {
        auto *var0 = new BinaryVariable(0, 0, 0.0);
        auto *var1 = new BinaryVariable(1, 0, 0.0);
        auto *var2 = new BinaryVariable(2, 0, 0.0);
        graph.owned_var_ptr_vec = {var0};
        graph.cached_var_ptr_vec = {var1, var2};
        auto *edge0 = new IdentityEdge(0, var0);
        auto *edge1 = new IdentityEdge(1, var1);
        auto *edge2 = new IdentityEdge(2, var0);
        auto *edge3 = new IdentityEdge(3, var2);
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0);
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0);
        var0->set_factor_vec({factor0, factor1});
    } else if (world.rank() == 1) {
        auto *var0 = new BinaryVariable(0, 0, 0.0);
        auto *var1 = new BinaryVariable(1, 0, 0.0);
        graph.owned_var_ptr_vec = {var1};
        graph.cached_var_ptr_vec = {var0};
        auto *edge0 = new IdentityEdge(0, var0);
        auto *edge1 = new IdentityEdge(1, var1);
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0);
        var1->set_factor_vec({factor0});
    } else if (world.rank() == 2) {
        auto *var0 = new BinaryVariable(0, 0, 0.0);
        auto *var2 = new BinaryVariable(2, 0, 0.0);
        graph.owned_var_ptr_vec = {var2};
        graph.cached_var_ptr_vec = {var0};
        auto *edge2 = new IdentityEdge(2, var0);
        auto *edge3 = new IdentityEdge(3, var2);
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0);
        var2->set_factor_vec({factor1});
    }
    while (num_samples--) {
        if (world.rank() == 0) { // master
            std::vector<int> val_vec(graph.owned_var_ptr_vec.size());
            for (size_t i = 0; i < val_vec.size(); ++i) val_vec[i] = graph.owned_var_ptr_vec[i]->get_value();
            for (auto worker_rank : workers_rank)
                world.send(worker_rank, static_cast<int>(MsgType::M_Send_B), val_vec);
            // master recieve the lastest variable values owned by workers;
            for (int worker_rank : workers_rank) {
                std::map<size_t, int> var_map;
                world.recv(worker_rank, static_cast<int>(MsgType::W_Send_D), var_map);
                for (auto &entry : var_map) {
                    // TODO careful with the corresponding relationships
                    const int shift = 1;
                    graph.cached_var_ptr_vec[entry.first - shift]->set_value(entry.second);
                }
            }
            // master resmaple the its owned variables
            for (auto &var : graph.owned_var_ptr_vec) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }
        } else {
            std::vector<int> val_vec(graph.cached_var_ptr_vec.size());
            world.recv(master_rank, static_cast<int>(MsgType::M_Send_B), val_vec);
            for (size_t i = 0; i < val_vec.size(); ++i) graph.cached_var_ptr_vec[i]->set_value(val_vec[i]);
            // workers perform a sample for the variable they owned
            for (auto &var : graph.owned_var_ptr_vec) {
                var->resample();
                counter[var->get_vid()][var->get_value()]++;
            }
            // workers send the value of variables they owned to the master
            std::map<size_t, int> var_map;
            for (auto var : graph.owned_var_ptr_vec) {
                var_map[var->get_vid()] = var->get_value();
            }
            world.send(master_rank, static_cast<int>(MsgType::W_Send_D), var_map);
        }

    }
    for (auto &entry : counter) {
        for (int v = 0; v < 2; ++v) {
            std::cout << "var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
        }
    }
}

