#include "Factor.h"
#include <iostream>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <unordered_map>

namespace mpi = boost::mpi;

enum class MessageType {
    VAR1_VAL, VAR2_VAL, VAR3_VAL
};

int main() {
//    auto *var1_ptr = new BinaryVariable(0, 0, 0.0);
//    auto *var2_ptr = new BinaryVariable(1, 0, 0.0);
//    auto *var3_ptr = new BinaryVariable(2, 0, 0.0);
//    auto *edge1_ptr = new IdentityEdge(0, var1_ptr);
//    auto *edge2_ptr = new IdentityEdge(1, var2_ptr);
//    auto *edge3_ptr = new IdentityEdge(2, var1_ptr);
//    auto *edge4_ptr = new IdentityEdge(3, var3_ptr);
//    std::vector<Edge *> factor1_edge_ptr_vec = {edge1_ptr, edge2_ptr};
//    auto *factor1_ptr = new AndFactor(factor1_edge_ptr_vec, 1.0);
//    std::vector<Edge *> factor2_edge_ptr_vec = {edge3_ptr, edge4_ptr};
//    auto *factor2_ptr = new AndFactor(factor2_edge_ptr_vec, 1.0);
//    var1_ptr->set_factor_vec({factor1_ptr, factor2_ptr});
//    var2_ptr->set_factor_vec({factor1_ptr});
//    var3_ptr->set_factor_vec({factor2_ptr});
//    std::vector<int> var1_count(2);
//    std::vector<int> var2_count(2);
//    std::vector<int> var3_count(2);
//
//    for (int i = 0; i < 100000; ++i) {
//        var1_ptr->resample();
//        var2_ptr->resample();
//        var3_ptr->resample();
//
//        var1_count[var1_ptr->get_value()]++;
//        var2_count[var2_ptr->get_value()]++;
//        var3_count[var3_ptr->get_value()]++;
//    }
//    std::cout << "var1=0: " << var1_count[0] << std::endl;
//    std::cout << "var1=1: " << var1_count[1] << std::endl; //78%
//    std::cout << "var2=0: " << var2_count[0] << std::endl;
//    std::cout << "var2=1: " << var2_count[1] << std::endl; //68%
//    std::cout << "var3=0: " << var3_count[0] << std::endl;
//    std::cout << "var3=1: " << var3_count[1] << std::endl; //68%
//
//    return 0;

    mpi::environment env;
    mpi::communicator world;
    size_t num_samples = 10000;


    auto *var1_ptr = new BinaryVariable(0, 0, 0.0);
    auto *var2_ptr = new BinaryVariable(1, 0, 0.0);
    auto *var3_ptr = new BinaryVariable(2, 0, 0.0);
    auto *edge1_ptr = new IdentityEdge(0, var1_ptr);
    auto *edge2_ptr = new IdentityEdge(1, var2_ptr);
    auto *edge3_ptr = new IdentityEdge(2, var1_ptr);
    auto *edge4_ptr = new IdentityEdge(3, var3_ptr);
    std::vector<Edge *> factor1_edge_ptr_vec = {edge1_ptr, edge2_ptr};
    auto *factor1_ptr = new AndFactor(factor1_edge_ptr_vec, 1.0);
    std::vector<Edge *> factor2_edge_ptr_vec = {edge3_ptr, edge4_ptr};
    auto *factor2_ptr = new AndFactor(factor2_edge_ptr_vec, 1.0);
    var1_ptr->set_factor_vec({factor1_ptr, factor2_ptr});
    var2_ptr->set_factor_vec({factor1_ptr});
    var3_ptr->set_factor_vec({factor2_ptr});
    std::unordered_map<size_t, std::vector<int>> counter;
    if (world.rank() == 0) {
        counter[1] = std::vector<int>(2);
        counter[2] = std::vector<int>(2);
        counter[3] = std::vector<int>(2);

    }
    while (num_samples--) {
        if (world.rank() == 0) { // master
            var1_ptr->resample();
            world.send(1, static_cast<int>(MessageType::VAR1_VAL), var1_ptr->get_value());
            //world.send(1, static_cast<int>(MessageType::VAR_ID), var1_ptr->get_vid());
            world.send(2, static_cast<int>(MessageType::VAR1_VAL), var1_ptr->get_value());
            //world.send(2, static_cast<int>(MessageType::VAR_ID), var1_ptr->get_vid());
            counter[1][var1_ptr->get_value()]++;
            int var2_value = 0;
            int var3_value = 0;
            world.recv(1, static_cast<int>(MessageType::VAR2_VAL), var2_value);
            var2_ptr->set_value(var2_value);
            world.recv(2, static_cast<int>(MessageType::VAR3_VAL), var3_value);
            var3_ptr->set_value(var3_value);
            counter[2][var2_ptr->get_value()]++;
            counter[3][var3_ptr->get_value()]++;
        } else {
            int var1_value = 0;
            world.recv(0, static_cast<int>(MessageType::VAR1_VAL), var1_value);
            var1_ptr->set_value(var1_value);
            if (world.rank() == 1) {
                var2_ptr->resample();
                world.send(0, static_cast<int>(MessageType::VAR2_VAL), var2_ptr->get_value());
            }
            if (world.rank() == 2) {
                var3_ptr->resample();
                world.send(0, static_cast<int>(MessageType::VAR3_VAL), var3_ptr->get_value());
            }
        }
    }
    if (world.rank() == 0) {
        for (auto &entry : counter) {
            auto val_vec = entry.second;
            for (size_t i = 0; i < val_vec.size(); ++i)
                std::cout << "var: " << entry.first << "=" << i << " " << val_vec[i] << std::endl;
        }
    }
}

