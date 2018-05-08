//
// Created by zq32 on 5/7/18.
//

#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include "FactorGraph.h"

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

    // construct the graph for three machine
    // construct the graph for three machine
    if (world.rank() == 0) { // master
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *edge0 = new IdentityEdge(0, var0, "D1");
        auto *edge2 = new IdentityEdge(2, var0, "D2");
        graph.var_ptr_map["B"] = {var0};
        auto *partial_fac0 = new PatialAndFactor(0, {edge0}, 1.0, "D1");
        auto *partial_fac1 = new PatialAndFactor(1, {edge2}, 1.0, "D2");
        graph.partial_factor_ptr_map["D1"] = {partial_fac0};
        graph.partial_factor_ptr_map["D2"] = {partial_fac1};
        var0->set_factor_vec({partial_fac0, partial_fac1});
    } else if (world.rank() == 1) { // worker1
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "C");
        auto *edge0 = new IdentityEdge(0, var0, "D");
        auto *edge1 = new IdentityEdge(1, var1, "C");
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0, "D");
        var1->set_factor_vec({factor0});
        graph.var_ptr_map["C"] = {var1};
        graph.var_ptr_map["B"] = {var0};
        graph.factor_ptr_map["D"] = {factor0};

    } else if (world.rank() == 2) {// worker2
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "C");
        auto *edge2 = new IdentityEdge(2, var0, "D");
        auto *edge3 = new IdentityEdge(3, var2, "C");
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0, "D");
        var2->set_factor_vec({factor1});
        graph.var_ptr_map["C"] = {var2};
        graph.var_ptr_map["B"] = {var0};
        graph.factor_ptr_map["D"] = {factor1};

    }

    while (num_samples--) {
        if (world.rank() == master_rank) { // master
            // Master broadcasts its assignment of all B-key variables to the workers
            if (graph.var_ptr_map.count("B")) {
                size_t master_B_var_size = graph.var_ptr_map["B"].size();
                std::vector<int> B_val_vec(master_B_var_size);
                for (size_t i = 0; i < master_B_var_size; ++i)
                    B_val_vec[i] = graph.var_ptr_map["B"][i]->get_value();
                for (auto worker_rank : workers_rank)
                    world.send(worker_rank, static_cast<int>(MsgType::M_Send_B), B_val_vec);
            }
            // Master computes partial evaluation of E_i-key factors, and transmits them to worker i
            // TODO
            // Master computes partial evaluation of G_i-key factors, and transmits them to worker i
            // TODO

            /********************* Master receive transmissions from workers ********************/
            // Master receive D_i variable from worker i, update its cache
            // TODO
            // Master receive D_i-key partial factor values from worker i, update its cache
            for (auto worker_rank : workers_rank) {
                std::string key = "D" + std::to_string(worker_rank);
                if (graph.partial_factor_ptr_map.count(key)) {
                    std::vector<float> D_pf_val_vec;
                    world.recv(worker_rank, static_cast<int>(MsgType::W_Send_P_D), D_pf_val_vec);
                    assert(D_pf_val_vec.size() == graph.partial_factor_ptr_map[key].size());
                    for (auto i = 0; i < D_pf_val_vec.size(); ++i)
                        graph.partial_factor_ptr_map[key][i]->set_partial_val(D_pf_val_vec[i]);
                }

            }
            // Master receive G_i-key partial factor values from worker i, update its cache
            // TODO


            /******************** Master perform Gibbs sampling on the variables it owns *******/
            // Master runs a single pass of Gibbs sampling on the A variable it owns.
            if (graph.var_ptr_map.count("A")) {
                for (auto& var_ptr : graph.var_ptr_map["A"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            // Master runs a single pass of Gibbs sampling on the B variable it owns.
            if (graph.var_ptr_map.count("B")) {
                for (auto& var_ptr : graph.var_ptr_map["B"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            /********************************  Master End *******************************************/
        } else { // for worker machines
            /********************* Worker receive transmissions from master ***********************/
            // Worker receive B variable value vector from master, update cached assignment
            if (graph.var_ptr_map.count("B")) {
                std::vector<int> B_val_vec;
                world.recv(master_rank, static_cast<int>(MsgType::M_Send_B), B_val_vec);
                for (auto i = 0; i < B_val_vec.size(); ++i)
                    graph.var_ptr_map["B"][i]->set_value(B_val_vec[i]);
            }
            // Worker receive E partial factor values from master, update cached assignment
            // TODO
            // Worker receive G partial factor values from master, update cached assignment
            // TODO

            /*********** Worker performs Gibbs sampling on the variable it owns *************/
            // Worker runs a single pass of Gibbs sampling on the C variables it owns
            if (graph.var_ptr_map.count("C")) {
                for (auto& var_ptr : graph.var_ptr_map["C"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            // Worker runs a single pass of Gibbs sampling on the D variable it owns
            if (graph.var_ptr_map.count("D")) {
                for (auto& var_ptr : graph.var_ptr_map["D"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            /****************************** Worker transmit to master ****************************/
            // Worker transmits its assignment of all D_i-key variables to the master
            if (graph.var_ptr_map.count("D")) {
                // TODO
            }
            /************************ Worker eval patial factor values ***************************/
            // Worker computes partial evaluation of all D_i-key factors, and transmits them to master
            if (graph.factor_ptr_map.count("D")) {
                size_t worker_D_pf_size = graph.factor_ptr_map["D"].size();
                std::vector<float> D_pf_vec(worker_D_pf_size);
                for (auto i = 0; i < worker_D_pf_size; ++i)
                    D_pf_vec[i] = graph.factor_ptr_map["D"][i]->partial_eval({"C"});
                world.send(master_rank, static_cast<int>(MsgType::W_Send_P_D), D_pf_vec);
            }
            // Worker computes partial evaluation of all G_i-key factors, and transmits them to master
            // TODO

        }

    }
    /****************** statistics *****************************************************/
    for (auto& entry : counter) {
        for (int v = 0; v < 2; ++v)
            std::cout << "var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
    }
    return 0;

}

