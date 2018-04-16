#include <algorithm>
#include <iostream>
#include <cmath>
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
    //std::cout<<"vid: "<<this->_vid<<", eng0: "<<energy_vec[0]<<", eng1: "<<energy_vec[1]<<std::endl;
    for (auto &energy : energy_vec) { energy = std::exp(energy); }

    _value = Utility::randomChoice(energy_vec);
}

BinaryVariable::BinaryVariable(size_t vid, int value, float prior_energy, std::string assign,
                               std::vector<Factor *> factor_ptr_vec) :
        Variable(vid, value, prior_energy, assign, factor_ptr_vec) {

}

BinaryVariable::BinaryVariable(size_t vid, int value, float prior_energy, std::string assign) :
        Variable(vid, value, prior_energy, assign, std::vector<Factor *>()) {

}


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

MultinomialVariable::MultinomialVariable(size_t vid, int value, unsigned int domain_size, float prior_energy,
                                         std::string assign,
                                         std::vector<Factor *> factor_ptr_vec) :
        Variable(vid, value, prior_energy, assign, factor_ptr_vec) {
    _domain_size = domain_size;
}

MultinomialVariable::MultinomialVariable(size_t vid, int value, unsigned int domain_size, float prior_energy,
                                         std::string assign) :
        Variable(vid, value, prior_energy, assign, std::vector<Factor *>()) {
    _domain_size = domain_size;
}


Variable::Variable(size_t vid, int value, float prior_energy, std::string assign,
                   std::vector<Factor *> factor_ptr_vec) :
        _vid(vid),
        _value(value),
        _prior_energy(prior_energy),
        _assign(assign),
        _factor_ptr_vec(std::make_unique<std::vector<Factor *>>(factor_ptr_vec)) {

}

Variable::Variable(size_t vid, int value, float prior_energy, std::string assign) :
        Variable(vid, value, prior_energy, assign, std::vector<Factor *>()) {

}
