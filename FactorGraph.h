#ifndef NUMBSKULL_FACTORGRAPH_H
#define NUMBSKULL_FACTORGRAPH_H

#include <vector>
#include <boost/serialization/map.hpp>
#include <fstream>
#include <memory>
#include "Variable.h"
#include "Factor.h"
#include "AssignInfo.h"

class FactorGraph {
public:
    FactorGraph();

    explicit FactorGraph(std::string conf);
    FactorGraph(size_t num_var, size_t num_fac,
                const std::map<std::string, std::shared_ptr<std::vector<Variable>>> &vars,
                const std::map<std::string, std::shared_ptr<std::vector<Factor>>> &facs,
                const std::map<std::string, AssignInfo> &ass_map);
    FactorGraph&operator=(const FactorGraph& other);
    friend std::ostream&operator<<(std::ostream& os, const FactorGraph& graph);

    bool parse_variables_file(std::string file_path);
    bool parse_factors_file(std::string file_path);


public:
    size_t num_variables = 0;
    size_t num_factors = 0;
    std::map<std::string, std::shared_ptr<std::vector<Variable>>> variables_map; // map<assign, variable>
    std::map<std::string, std::shared_ptr<std::vector<Factor>>> factors_map; // map<assign, factor>
    std::map<std::string, AssignInfo> assign_map;

};


#endif //NUMBSKULL_FACTORGRAPH_H
