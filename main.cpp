#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "FactorGraph.h"

namespace mpi = boost::mpi;
namespace mpl = boost::mpl;

enum class MessageType {
    Info, Var, Fac
};

int num_samples = 1;

int randomChoice(std::vector<int> values, std::vector<double> probs) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> sampler(probs.begin(), probs.end());
    int sample_idx = sampler(gen);
    return values[sample_idx];
}

int main() {
    mpi::environment env;
    mpi::communicator world;

    // TODO
    std::map<size_t, std::vector<size_t>> send_map;
    send_map[0] = {1, 2};
    send_map[1] = {0};
    send_map[2] = {0};

    if (world.rank() == 0) { // master
        FactorGraph graph("../Input/GraphInfo");
        graph.parse_variables_file("../Input/Variables");
        graph.parse_factors_file("../Input/Factors");

        std::cout << graph << std::endl;
        for (auto &entry : graph.assign_map) {
            std::string assign = entry.first;
            AssignInfo info = entry.second;
            int machine_index = assign[1] - '0'; // for example, we have B0, C1, machine index is 0, 1
            world.send(machine_index, static_cast<int>(MessageType::Info), info);

            if (info.num_variables > 0) {
                size_t var_end_idx = info.var_start_idx + info.num_variables;
                std::vector<Variable> vec_vars = std::vector<Variable>(graph.variables_map[assign]->begin(),
                                                                       graph.variables_map[assign]->begin() +
                                                                       info.num_variables);
                //std::cout << "try send variables to " << machine_index << std::endl;
                //for (auto& var : vec_vars) std::cout << var << std::endl;
                world.send(machine_index, static_cast<int>(MessageType::Var), vec_vars);
                //std::cout << "send variables to " << machine_index << " finished" << std::endl;
            }


            if (info.num_factors > 0) {
                size_t fac_end_idx = info.fac_start_idx + info.num_factors;
                std::vector<Factor> vec_facs = std::vector<Factor>(graph.factors_map[assign]->begin(),
                                                                   graph.factors_map[assign]->begin() +
                                                                   info.num_factors);
                //std::cout << "try send factors to " << machine_index << std::endl;
                //for (auto& fac : vec_facs) std::cout << fac << std::endl;
                world.send(machine_index, static_cast<int>(MessageType::Fac), vec_facs);
                //std::cout << "send factors to " << machine_index << " finished" << std::endl;
            }
        }
    }

    //std::cout << "rank " << world.rank() << " ready to receive" << std::endl;
    AssignInfo conf_info;
    std::vector<Factor> facs;
    std::vector<Variable> vars;
    world.recv(0, static_cast<int>(MessageType::Info), conf_info);
    if (conf_info.num_factors > 0) {
        world.recv(0, static_cast<int>(MessageType::Fac), facs);
        //std::cout << "worker" << world.rank() << "receive factors" << std::endl;
        //for (auto fac : facs) std::cout << fac << std::endl;
    }

    if (conf_info.num_variables > 0) {
        world.recv(0, static_cast<int>(MessageType::Var), vars);
        //std::cout << "worker " << world.rank() << " receive variables" << std::endl;
        //for (auto var : vars) std::cout << var << std::endl;
    }


    while (--num_samples) {
        if (world.rank() == 0) {
            // master node send variables to worker nodes
            for (auto machine_idx : send_map[world.rank()]) {
                world.send(machine_idx, static_cast<int>(MessageType::Var), vars);
            }
            // wait for worker nodes' response


        } else {
            // receive variables from master node
            // TODO from where, start index
            size_t var_start_idx = 0;
            std::vector<Variable> rev_vars;
            world.recv(0, static_cast<int>(MessageType::Var), rev_vars);

            for (auto &var : vars) {
                std::map<int, double> var_prob = {{0, 0.0},
                                                  {0, 0.0}};
                for (auto &fid : var.factors) {
                    Factor &factor = facs[fid - conf_info.fac_start_idx];
                    // TODO a factor connected more than 2 variables?
                    size_t other_vid = factor.variables[0] == var.vid ? factor.variables[1] ? factor.variables[0];
                    int other_var_val = rev_vars[other_vid - var_start_idx].value;
                    for (auto &entry : var_prob) {
                        if (factor.type == "EQU") {
                            var_prob[entry.first] += other_var_val == var.value ? factor.weight : 0;
                        }
                            // TODO support other factor type
                        else exit(-1);
                    }
                }
                std::for_each(var_prob.begin(), var_prob.end(), [](auto& entry){entry.second = std::exp(entry.second);});
                std::vector<int> var_val;
                std::vector<double> probs;
                for (auto& entry : var_prob) {
                    var_val.push_back(entry.first);
                    probs.push_back(entry.second);
                }
                int new_val = randomChoice(var_val, probs);
                var.value = new_val;
            }
            // worker calculate partial result for variables in class B
            std::map<size_t, std::map<int, double>> B_var_probs;
            for (auto& entry : rev_vars) {
                
            }

            // worker nodes send combined result to master node

            // wait for
        }
    }

    return 0;
}

