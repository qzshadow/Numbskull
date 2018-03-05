#include "Variable.h"


void BinaryVariable::resample() {

}

BinaryVariable::BinaryVariable(bool val, float prior_energy, std::vector<size_t> fac_id_vec) :
        _value(val), _prior_energy(prior_energy), _fac_id_vec(fac_id_vec){
}

void MultinomialVariable::resample() {

}

MultinomialVariable::MultinomialVariable(bool val, float prior_energy, std::vector<size_t> fac_id_vec) :
        _value(val), _prior_energy(prior_energy) , _fac_id_vec(fac_id_vec){
}
