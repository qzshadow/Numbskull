


#include <boost/shared_ptr.hpp>
#include "FactorGraph.h"

const std::shared_ptr<std::vector<Variable *>> &FactorGraph::get_var_ptr_vec() const {
    return _var_ptr_vec;
}

void FactorGraph::set_var_ptr_vec(const std::shared_ptr<std::vector<Variable *>> &_var_ptr_vec) {
    FactorGraph::_var_ptr_vec = _var_ptr_vec;
}

const std::shared_ptr<std::vector<Edge *>> &FactorGraph::get_edge_ptr_vec() const {
    return _edge_ptr_vec;
}

void FactorGraph::set_edge_ptr_vec(const std::shared_ptr<std::vector<Edge *>> &_edge_ptr_vec) {
    FactorGraph::_edge_ptr_vec = _edge_ptr_vec;
}

const std::shared_ptr<std::vector<Factor *>> &FactorGraph::get_factor_ptr_vec() const {
    return _factor_ptr_vec;
}

void FactorGraph::set_factor_ptr_vec(const std::shared_ptr<std::vector<Factor *>> &_factor_ptr_vec) {
    FactorGraph::_factor_ptr_vec = _factor_ptr_vec;
}
