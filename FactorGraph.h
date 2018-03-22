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


    const std::shared_ptr<std::vector<Variable *>> &get_var_ptr_vec() const;

    void set_var_ptr_vec(const std::shared_ptr<std::vector<Variable *>> &_var_ptr_vec);

    const std::shared_ptr<std::vector<Edge *>> &get_edge_ptr_vec() const;

    void set_edge_ptr_vec(const std::shared_ptr<std::vector<Edge *>> &_edge_ptr_vec);

    const std::shared_ptr<std::vector<Factor *>> &get_factor_ptr_vec() const;

    void set_factor_ptr_vec(const std::shared_ptr<std::vector<Factor *>> &_factor_ptr_vec);

private:
    std::unordered_map<std::string, size_t> _var_map;
    std::shared_ptr<std::vector<Edge *>> _edge_ptr_vec;
    std::unordered_map<std::string, size_t> _edge_map;
    std::shared_ptr<std::vector<Factor *>> _factor_ptr_vec;
    std::shared_ptr<std::vector<Variable *>> _var_ptr_vec;
    std::unordered_map<std::string, size_t> _factor_map;
};


#endif //NUMBSKULL_FACTORGRAPH_H
