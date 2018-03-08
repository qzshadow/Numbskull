#include "Factor.h"


float AndFactor::eval() {
    bool result = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        result = result && edge_ptr->transform();
    }
    return result * _weight;
}

AndFactor::AndFactor(std::vector<Edge *> edge_ptr_vec, float weight) :
        _edge_ptr_vec(std::make_unique<std::vector<Edge *>>(edge_ptr_vec)),
        _weight(weight) {}
