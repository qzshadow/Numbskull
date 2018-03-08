/*
 * FactorGraph class
 * This class is used to generate the graph from input file
 * Contains information of the whole graph
 * and how to assign the graph to different machines
 */
#ifndef NUMBSKULL_FACTORGRAPH_H
#define NUMBSKULL_FACTORGRAPH_H

#include <memory>
#include "Variable.h"
#include "Edge.h"
#include "Factor.h"
#include <vector>

class FactorGraph {
public:
    Variable &get_variable_by_vid(size_t vid);

    Edge &get_edge_by_eid(size_t eid);

    Factor &get_factor_by_fid(size_t fid);

private:
    std::unique_ptr<std::vector<Variable>> _variable_vec;
    std::unique_ptr<std::vector<Edge>> _edge_vec;
    std::unique_ptr<std::vector<Factor>> _factor_vec;

};


#endif //NUMBSKULL_FACTORGRAPH_H
