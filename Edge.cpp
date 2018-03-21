//
// Created by zq32 on 3/4/18.
//

#include "Edge.h"

float NegateEdge::transform() {
//    _edge_val = -_var->get_value();
//    return _edge_val;
    return -_var->get_value();
}

NegateEdge::NegateEdge(size_t eid, Variable *var) :
        _var(var), _eid(eid) {

}

float IdentityEdge::transform() {
//    _edge_val = _var->get_value();
//    return _edge_val;
    return _var->get_value();
}

IdentityEdge::IdentityEdge(size_t eid, Variable *var) :
        _var(var), _eid(eid) {

}
