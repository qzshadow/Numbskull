#include "Factor.h"


float AndFactor::eval() {
    bool result = true;
    for (auto &edge_ptr : *_edge_ptr_vec) {
        result = result && edge_ptr->transform();
    }
    return result * _weight;
}
