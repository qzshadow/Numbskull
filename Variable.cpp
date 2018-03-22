#include <algorithm>
#include <iostream>
#include "Variable.h"
#include "Factor.h"
#include "Utility.h"


void BinaryVariable::resample() {
    //std::cout<<"resample called"<<std::endl;
    std::vector<float> energy_vec(2);
    // std::cout << _factor_ptr_vec->size() << std::endl;
    for (auto &factor_ptr : *_factor_ptr_vec) {
        for (int value = 0; value < 2; ++value) {
            _value = value;
            energy_vec[value] += factor_ptr->eval();
        }
    }
    for (auto &energy : energy_vec) { energy = std::exp(energy); }
    _value = Utility::randomChoice(energy_vec);
}

BinaryVariable::BinaryVariable(size_t vid, int value, float prior_energy, std::vector<Factor *> factor_ptr_vec) :
        _vid(vid), _value(value), _prior_energy(prior_energy),
        _factor_ptr_vec(std::make_unique<std::vector<Factor *>>(factor_ptr_vec)) {

}

BinaryVariable::BinaryVariable(size_t vid, int value, float prior_energy) :
        BinaryVariable(vid, value, prior_energy, std::vector<Factor *>()) {

}

//BinaryVariable::BinaryVariable(bool val, float prior_energy, size_t fid_begin_idx, size_t fid_size) :
//        _value(val), _prior_energy(prior_energy), _fid_begin_idx(fid_begin_idx), _fid_size(fid_size){
//}

void MultinomialVariable::resample() {
    std::vector<float> energy_vec(_domain_size);
    for (auto &factor_ptr : *_factor_ptr_vec) {
        for (int value = 0; value < _domain_size; ++value) {
            _value = value;
            energy_vec[value] += factor_ptr->eval();
        }
    }
    for (auto &energy : energy_vec) { energy = std::exp(energy); }
    _value = Utility::randomChoice(energy_vec);

}

MultinomialVariable::MultinomialVariable(size_t vid, int value, float prior_energy,
                                         std::vector<Factor *> factor_ptr_vec) :
        _vid(vid), _value(value), _prior_energy(prior_energy),
        _factor_ptr_vec(std::make_unique<std::vector<Factor *>>(factor_ptr_vec)) {

}

MultinomialVariable::MultinomialVariable(size_t vid, int value, float prior_energy) :
        MultinomialVariable(vid, value, prior_energy, std::vector<Factor *>()) {}


//MultinomialVariable::MultinomialVariable(bool val, float prior_energy, size_t fid_begin_idx, size_t fid_size) :
//        _value(val), _prior_energy(prior_energy) , _fid_begin_idx(fid_begin_idx), _fid_size(fid_size){
//}
