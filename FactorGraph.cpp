#include <iostream>
#include "FactorGraph.h"


Variable &FactorGraph::get_variable_by_vid(size_t vid) {
    return _variable_vec->at(vid);
}

Edge &FactorGraph::get_edge_by_eid(size_t eid) {
    return _edge_vec->at(eid);
}

Factor &FactorGraph::get_factor_by_fid(size_t fid) {
    return _factor_vec->at(fid);
}
