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
#include <map>

class FactorGraph {
public:


private:
    Variable **_var_vec_ptr;
    std::map<std::string, size_t> _var_map;
    Edge **_edge_vec_ptr;
    std::map<std::string, size_t> _edge_map;
    Factor **_factor_vec_ptr;
    std::map<std::string, size_t> _factor_map;
};


#endif //NUMBSKULL_FACTORGRAPH_H
