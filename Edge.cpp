//
// Created by zq32 on 3/4/18.
//

#include "Edge.h"

float NegateEdge::transform() {
//    _edge_val = -_var->get_value();
//    return _edge_val;
    return -_var->get_value();
}

//NegateEdge::NegateEdge(Variable* var, size_t eid, float edge_val) :
//_var(var), _eid(eid), _edge_val(edge_val){
//
//}

float IdentityEdge::transform() {
//    _edge_val = _var->get_value();
//    return _edge_val;
    return _var->get_value();
}
