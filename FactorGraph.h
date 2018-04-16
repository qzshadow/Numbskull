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
#include <unordered_map>

class FactorGraph {
public:
    std::unordered_map<std::string, std::vector<Factor *>> factor_ptr_map;
    //std::unordered_map<std::string, std::vector<Factor *>> cached_factor_ptr_map;
    std::unordered_map<std::string, std::vector<PatialFactor *>> partial_factor_ptr_map;

    std::unordered_map<std::string, std::vector<Variable *>> var_ptr_map;
    //std::unordered_map<std::string, std::vector<Variable *>> cached_var_ptr_map;
};


#endif //NUMBSKULL_FACTORGRAPH_H
