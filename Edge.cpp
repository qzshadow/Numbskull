//
// Created by zq32 on 3/4/18.
// Edge class
//

#include "Edge.h"

float NegateEdge::transform() {
    return -_var->get_value();
}

NegateEdge::NegateEdge(size_t eid, Variable *var, std::string assign) :
        Edge(eid, var, assign) {

}

float IdentityEdge::transform() {
    return _var->get_value();
}

IdentityEdge::IdentityEdge(size_t eid, Variable *var, std::string assign) :
        Edge(eid, var, assign) {

}

Edge::Edge(size_t eid, Variable *var, std::string assign) : _var(var), _eid(eid), _assign(assign) {

}
