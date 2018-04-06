#include <iostream>
#include "Factor.h"
#include <algorithm>

Factor::Factor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        _fid(fid),
        _edge_ptr_vec(std::make_unique<std::vector<Edge *>>(edge_ptr_vec)),
        _weight(weight) {
}

float AndFactor::eval() {
    bool result = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        result = result && edge_ptr->transform();
        if (!result) break;
    }
    //std::cout<<result*_weight<<std::endl;
    return result * _weight;
}

AndFactor::AndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        Factor(fid, edge_ptr_vec, weight) {}

float AndFactor::partial_eval(std::vector<Variable *> &owned_var_ptr_vec) {
    bool partial_res = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        Variable *var_ptr = edge_ptr->get_var();
        // TODO optimize the complexity
        if (std::find(owned_var_ptr_vec.begin(), owned_var_ptr_vec.end(), var_ptr)
            != owned_var_ptr_vec.end()) { // the variable is owned by this machine
            partial_res = partial_res && edge_ptr->transform();
        }
    }
    return partial_res;
}

PatialAndFactor::PatialAndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        PatialFactor(fid, edge_ptr_vec, weight) {}

float PatialAndFactor::eval() {
    //std::cout<<_patial_val<<std::endl;
    bool res = _patial_val;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        res = res && edge_ptr->transform();
        if (!res) break;
    }
    return res * _weight;
}

PatialFactor::PatialFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        Factor(fid, edge_ptr_vec, weight) {}
