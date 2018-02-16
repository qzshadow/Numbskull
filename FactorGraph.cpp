#include <iostream>
#include "FactorGraph.h"

FactorGraph::FactorGraph(size_t num_var, size_t num_fac,
                         const std::map<std::string, std::shared_ptr<std::vector<Variable>>> &vars,
                         const std::map<std::string, std::shared_ptr<std::vector<Factor>>> &facs,
                         const std::map<std::string, AssignInfo> &ass_map)
        : num_variables(num_var),
          num_factors(num_fac),
          variables_map(vars),
          factors_map(facs),
          assign_map(ass_map) {}

FactorGraph &FactorGraph::operator=(const FactorGraph &other) {
    num_variables = other.num_variables;
    num_factors = other.num_factors;
    variables_map = other.variables_map;
    factors_map = other.factors_map;
    assign_map = other.assign_map;
    return *this;
}

bool FactorGraph::parse_variables_file(std::string file_path) {
    // Variable file format
    // vid assign value factor
    // 0 B 0 1
    std::ifstream in_file(file_path, std::ios_base::in);
    if (in_file.is_open()) {
        size_t vid;
        std::string assign;
        double value;
        size_t fid;
        while (in_file >> vid >> assign >> value >> fid) {
            size_t sv = assign_map[assign].var_start_idx;
            size_t sf = assign_map[assign].fac_start_idx;
            if (variables_map[assign]->at(vid - sv).vid == SIZE_MAX)
                variables_map[assign]->at(vid - sv) = Variable(vid, assign, value,
                                                                             std::vector<size_t>({fid}));
            else variables_map[assign]->at(vid - sv).factors.push_back(fid);
        }
        return true;
    }
    return false;
}

bool FactorGraph::parse_factors_file(std::string file_path) {
    // Factor file format
    // fid  assign  vid factor_type weight
    // 1    D1      0   EQU     0.9
    std::ifstream in_file(file_path, std::ios_base::in);
    if (in_file.is_open()) {
        size_t fid;
        std::string assign;
        size_t vid;
        std::string type;
        double weight;
        while (in_file >> fid >> assign >> vid >> type >> weight) {
            size_t sv = assign_map[assign].var_start_idx;
            size_t sf = assign_map[assign].fac_start_idx;
            if (factors_map[assign]->at(fid - sf).fid == SIZE_MAX)
                factors_map[assign]->at(fid - sf) = Factor(fid, assign, std::vector<size_t>({vid}), type,
                                                                         weight);
            else factors_map[assign]->at(fid - sf).variables.push_back(vid);
        }
        return true;
    }
    return false;
}

FactorGraph::FactorGraph(std::string conf) {
    // GraphInfo
    // assign num_var num_fac
    // B    1   0
    std::ifstream in_file(conf, std::ios_base::in);
    if (in_file.is_open()) {
        std::string assign;
        size_t var_idx;
        size_t num_var;
        size_t fac_idx;
        size_t num_fac;
        while (in_file >> assign >> var_idx >> num_var >> fac_idx >> num_fac) {
            assign_map[assign] = AssignInfo(var_idx, num_var, fac_idx, num_fac);
            num_variables += num_var;
            num_factors += num_fac;
            if (num_var > 0) variables_map[assign] = std::make_shared<std::vector<Variable>>(std::vector<Variable>(num_var));
            if (num_fac > 0) factors_map[assign] = std::make_shared<std::vector<Factor>>(std::vector<Factor>(num_fac));
        }
    }
}

std::ostream &operator<<(std::ostream &os, const FactorGraph &graph) {
    os << "Factor Number: " << graph.num_factors << std::endl;
       os<< "Variable Number: " << graph.num_variables << std::endl;
    return os;
}

FactorGraph::FactorGraph() = default;
