//
// Created by zq32 on 2/25/18.
//

#include "FactorGraph.h"
#include <boost/mpi/communicator.hpp>
#include "Utility.h"


int Utility::randomChoice(std::vector<int> values, std::vector<float> probs) {
    int sample_idx = Utility::randomChoice(probs);
    return values[sample_idx];
}

//int Utility::randomChoice(std::map<int, float> val_probs) {
//    std::vector<int> var_val;
//    std::vector<float> probs;
//    for (auto &val_prob : val_probs) {
//        var_val.push_back(val_prob.first);
//        probs.push_back(val_prob.second);
//    }
//    return Utility::randomChoice(var_val, probs);
//
//}

int Utility::randomChoice(std::vector<float> energy_vec) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> sampler(energy_vec.begin(), energy_vec.end());
    return sampler(gen);
}


