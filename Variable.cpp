#include <algorithm>
#include "Variable.h"
#include "Factor.h"
#include "Utility.h"


void BinaryVariable::resample() {
    std::vector<float> energy_vec(2);
    for (auto &factor_ptr : *_factor_ptr_vec) {
        energy_vec[_value] += factor_ptr->eval();
    }
    for (auto &energy : energy_vec) { energy = std::exp(energy); }
    _value = Utility::randomChoice(energy_vec);
}

//BinaryVariable::BinaryVariable(bool val, float prior_energy, size_t fid_begin_idx, size_t fid_size) :
//        _value(val), _prior_energy(prior_energy), _fid_begin_idx(fid_begin_idx), _fid_size(fid_size){
//}

void MultinomialVariable::resample() {
    std::vector<float> energy_vec(_domain_size);
    for (auto &factor_ptr : *_factor_ptr_vec) {
        energy_vec[_value] += factor_ptr->eval();
    }
    for (auto &energy : energy_vec) { energy = std::exp(energy); }
    _value = Utility::randomChoice(energy_vec);

}

//MultinomialVariable::MultinomialVariable(bool val, float prior_energy, size_t fid_begin_idx, size_t fid_size) :
//        _value(val), _prior_energy(prior_energy) , _fid_begin_idx(fid_begin_idx), _fid_size(fid_size){
//}
