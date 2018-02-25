#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "FactorGraph.h"
#include "Utility.h"


namespace mpi = boost::mpi;
namespace mpl = boost::mpl;

/* different message types, used as the tag when transform messages bettween machines.
* Info : this message used to send AssignInfo from master machine to all machines (include itself)
* Var : this message used to transfer vars between master machine and workers
* Fac : this message used to transfer facs between master machine and workers
* Comb : this message used to transfer Combined energy (probs in the code) from workers to master
* Cnt : this message used to transfer count info for each variable on workers to master machine
 */
enum class MessageType {
    Info, Var, Fac, Comb, Cnt
};
// sampling number for each variables
int num_samples = 10000;

int main() {
    mpi::environment env;
    mpi::communicator world;

    // TODO this map indicates to which machine will this machine send its message
    // This should be generated when we construct the graph following the principle in the paper
    // For this simple BDC graph try, I construct it artificially
    std::map<size_t, std::vector<size_t>> send_map;
    send_map[0] = {1, 2};
    send_map[1] = {0};
    send_map[2] = {0};

    // This map is used to count the number of values for variables
    // is mainly used to verify the correctness of the algorithm.
    std::map<size_t, std::map<int, int>> count_map;

    if (world.rank() == 0) { // master do the assign work
        // TODO parse the graph from database with sql
        /* Firstly the master node construct the graph by parsing the input file
        * In this demo, the graph contains 3 machines, master machine 0, worker machine 1, 2
        * master machine 0 contains 2 variables their ids are 0 and 1
        * worker machine 1 contains 2 variables their ids are 2 and 3
        * worker machine 2 contains 2 variables their ids are 4 and 5
        * worker machine 1 contains 4 factors their ids are 0 1 2 3
        * worker machine 2 contains 4 factors their ids are 4 5 6 7
        * see README.md for the topological picture
        */
        FactorGraph graph("../Input/GraphInfo");
        graph.parse_variables_file("../Input/Variables");
        graph.parse_factors_file("../Input/Factors");

        std::cout << graph << std::endl;
        /* machine_map is a litter diff from assign_map
        * The key of assign_map is like "B0"
         * The key of machine_map is the machine number 0
         * machine_map will aggregate the info in assign_map then send it to different machine
         */
        std::map<size_t, AssignInfo> machine_map;

        for (auto &entry : graph.assign_map) {
            size_t machine_idx = entry.first[1] - '0';
            AssignInfo &info = entry.second;
            if (machine_map.count(machine_idx) == 0) machine_map[machine_idx] = info;
            else {
                AssignInfo &origin = machine_map[machine_idx];
                // check if there was a conflict in assign_map
                if (origin.num_factors != 0 && info.num_factors != 0
                    || origin.var_start_idx != 0 && info.var_start_idx != 0
                    || origin.num_variables != 0 && info.num_variables != 0
                    || origin.fac_start_idx != 0 && info.fac_start_idx != 0)
                    exit(-1);
                origin.num_variables += info.num_variables;
                origin.num_factors += info.num_factors;
                origin.var_start_idx += info.var_start_idx;
                origin.fac_start_idx += info.fac_start_idx;
            }
        }
        for (auto &entry : machine_map) {
            world.send(entry.first, static_cast<int>(MessageType::Info), entry.second);
        }
        for (auto &entry : graph.assign_map) {
            std::string assign = entry.first;
            int machine_index = assign[1] - '0'; // for example, we have B0, C1, machine index is 0, 1
            AssignInfo info = entry.second;

            // if this machine own variables, receieve the variables from master
            if (info.num_variables > 0) {
                std::vector<Variable> vec_vars = std::vector<Variable>(graph.variables_map[assign]->begin(),
                                                                       graph.variables_map[assign]->begin() +
                                                                       info.num_variables);
                world.send(machine_index, static_cast<int>(MessageType::Var), vec_vars);
            }

            // if this machine own factors, receieve the factors from master
            if (info.num_factors > 0) {
                std::vector<Factor> vec_facs = std::vector<Factor>(graph.factors_map[assign]->begin(),
                                                                   graph.factors_map[assign]->begin() +
                                                                   info.num_factors);
                world.send(machine_index, static_cast<int>(MessageType::Fac), vec_facs);
            }
        }
    }

    // all machine will receive info, factors and variables from master (include master itself)
    AssignInfo conf_info;
    std::vector<Factor> facs;
    std::vector<Variable> vars;
    world.recv(0, static_cast<int>(MessageType::Info), conf_info);
    if (conf_info.num_factors > 0) {
        world.recv(0, static_cast<int>(MessageType::Fac), facs);
    }

    if (conf_info.num_variables > 0) {
        world.recv(0, static_cast<int>(MessageType::Var), vars);
    }



    // Sampling start for this BDC graph
    while (num_samples--) {
        if (world.rank() == 0) { // master
            // master node send variables to worker nodes
            for (auto machine_idx : send_map[world.rank()]) {
                world.send(machine_idx, static_cast<int>(MessageType::Var), vars);
            }
            // wait for worker nodes' response
            std::map<size_t, std::map<int, double>> B_var_probs; // map<vid, map<value, energy>>
            for (auto machine_idx : send_map[world.rank()]) {
                std::map<size_t, std::map<int, double>> partial_B_var_probs;
                // recieve the aggregated energys from workers
                world.recv(machine_idx, static_cast<int>(MessageType::Comb), partial_B_var_probs);

                // aggregate the partial result again
                for (auto &entry : partial_B_var_probs) {
                    if (B_var_probs.count(entry.first) == 0) {
                        B_var_probs[entry.first] = entry.second;

                    } else {
                        for (auto &val_prob : entry.second) {
                            B_var_probs[entry.first][val_prob.first] += val_prob.second;
                        }
                    }
                }
            }

            // sampling the variables on the master
            for (auto &entry : B_var_probs) {
                // calculate the energy
                for (auto &val_probs : entry.second) val_probs.second = std::exp(val_probs.second);
                int new_val = Utility::randomChoice(entry.second);
                vars[entry.first - conf_info.var_start_idx].value = new_val;
                count_map[entry.first][new_val]++;
            }


        } else { // worker node
            // receive variables from master node
            // TODO from where, start index
            size_t var_start_idx = 0;
            std::vector<Variable> recv_vars;
            world.recv(0, static_cast<int>(MessageType::Var), recv_vars);
            for (auto &var : vars) {
                // TODO variable has more than 2 value?
                std::map<int, double> val_prob = {{0, 0.0},
                                                  {1, 0.0}};
                for (auto &fid : var.factors) {
                    Factor &factor = facs[fid - conf_info.fac_start_idx];
                    // TODO a factor connected more than 2 variables?
                    size_t other_vid = factor.variables[0] == var.vid ? factor.variables[1] : factor.variables[0];
                    int other_var_val = recv_vars[other_vid - var_start_idx].value;
                    for (auto &entry : val_prob) {
                        if (factor.type == "EQU") {
                            entry.second += other_var_val == entry.first ? factor.weight : 0;
                        }
                            // TODO support other factor type
                        else exit(-1);
                    }
                }
                // calculate the energy
                for (auto &entry : val_prob) entry.second = std::exp(entry.second);
                int new_val = Utility::randomChoice(val_prob);
                var.value = new_val;
                count_map[var.vid][new_val]++;
            }

            // aggregate variable energy info to transfer back to master
            std::map<size_t, std::map<int, double>> B_var_probs;
            for (auto &var : recv_vars) {
                size_t vid = var.vid;
                //TODO the variable contains more than 2 values?
                B_var_probs[vid] = {{0, 0.0},
                                    {1, 0.0}};
                std::vector<size_t> fac_ids = var.factors;
                for (size_t fid : fac_ids) {
                    if (fid >= conf_info.fac_start_idx && fid < conf_info.fac_start_idx + conf_info.num_factors) {
                        Factor &factor = facs[fid - conf_info.fac_start_idx];
                        // TODO a factor connected more than 2 variables?
                        size_t other_vid = (factor.variables[0] == vid ? factor.variables[1] : factor.variables[0]);
                        int other_var_val = vars[other_vid - var_start_idx].value;

                        for (auto &entry : B_var_probs[vid]) {
                            if (factor.type == "EQU") {
                                entry.second += other_var_val == entry.first ? factor.weight : 0.0;
                            }
                                // TODO support other factor types
                            else exit(-1);
                        }
                    }
                }
            }
            world.send(0, static_cast<int>(MessageType::Comb), B_var_probs);

        }
    }

    // print the sample count for each variable on master
    if (world.rank() == 0) {
        for (auto &var_entry : count_map) {
            for (auto &val_count : var_entry.second) {
                std::cout << "var " << var_entry.first << " val " << val_count.first << " count " << val_count.second
                          << std::endl;
            }
        }
        std::map<size_t, std::map<int, int>> worker_count_map;
        for (size_t machine_idx : send_map[0]) {
            world.recv(machine_idx, static_cast<int>(MessageType::Cnt), worker_count_map);
            for (auto &var_entry : worker_count_map) {
                for (auto &val_count : var_entry.second) {
                    std::cout << "var " << var_entry.first << " val " << val_count.first << " count "
                              << val_count.second << std::endl;
                }
            }
        }
    } else {
        world.send(0, static_cast<int>(MessageType::Cnt), count_map);
    }


    return 0;
}

