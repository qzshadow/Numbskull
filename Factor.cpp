#include <iostream>
#include "Factor.h"
#include <algorithm>

Factor::Factor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        Factor::Factor(fid, edge_ptr_vec, weight, "") {
}

Factor::Factor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign) :
        _fid(fid),
        _edge_ptr_vec(std::make_unique<std::vector<Edge *>>(edge_ptr_vec)),
        _weight(weight),
        _assign(assign) {

}

float AndFactor::eval() {
    bool result = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        result = result && edge_ptr->transform();
        if (!result) break;
    }
    return result * _weight;
}

AndFactor::AndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        Factor(fid, edge_ptr_vec, weight) {}

float AndFactor::partial_eval(std::unordered_set<std::string> own_var_set) {
    bool partial_res = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        Variable *var_ptr = edge_ptr->get_var();
        if (own_var_set.count(var_ptr->get_assign())) { // the variable is owned by this machine
            partial_res = partial_res && edge_ptr->transform();
        }
    }

    return partial_res;
}

AndFactor::AndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign) :
        Factor(fid,
               edge_ptr_vec,
               weight,
               assign) {

}

PartialAndFactor::PartialAndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        PartialFactor(fid, edge_ptr_vec, weight),
        AndFactor(fid, edge_ptr_vec, weight),
    Factor(fid, edge_ptr_vec, weight){}

float PartialAndFactor::eval() {
    bool res = _patial_val;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        res = res && edge_ptr->transform();
        if (!res) break;
    }
    return res * _weight;
}

PartialAndFactor::PartialAndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign)
        : PartialFactor(fid, edge_ptr_vec, weight, assign),
            AndFactor(fid, edge_ptr_vec, weight, assign),
            Factor(fid, edge_ptr_vec, weight, assign){

}

PartialFactor::PartialFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight) :
        Factor(fid, edge_ptr_vec, weight) {}

PartialFactor::PartialFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign) :
        Factor(fid, edge_ptr_vec,weight, assign) {}
