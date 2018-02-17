#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>

#include "FactorGraph.h"

namespace mpi = boost::mpi;
namespace mpl = boost::mpl;

enum class MessageType {
    Info, Var, Fac
};

int main() {
    mpi::environment env;
    mpi::communicator world;
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
                std::cout << "try send variables to " << machine_index << std::endl;
                for (auto var : vec_vars) std::cout << var << std::endl;
                world.send(machine_index, static_cast<int>(MessageType::Var), vec_vars);
                std::cout << "send variables to " << machine_index << " finished" << std::endl;
            }


            if (info.num_factors > 0) {
                size_t fac_end_idx = info.fac_start_idx + info.num_factors;
                std::vector<Factor> vec_facs = std::vector<Factor>(graph.factors_map[assign]->begin(),
                                                                   graph.factors_map[assign]->begin() +
                                                                   info.num_factors);
                std::cout << "try send factors to " << machine_index << std::endl;
                for (auto fac : vec_facs) std::cout << fac << std::endl;
                world.send(machine_index, static_cast<int>(MessageType::Fac), vec_facs);
                std::cout << "send factors to " << machine_index << " finished" << std::endl;
            }
        }
    }

    std::cout << "rank " << world.rank() << " ready to receive" << std::endl;
    AssignInfo info;
    world.recv(0, static_cast<int>(MessageType::Info), info);
    if (info.num_factors > 0) {
        std::vector<Factor> facs;
        world.recv(0, static_cast<int>(MessageType::Fac), facs);
        std::cout << "worker" << world.rank() << "receive factors" << std::endl;
        for (auto fac : facs) std::cout << fac << std::endl;
    }

    if (info.num_variables > 0) {
        std::vector<Variable> vars;
        world.recv(0, static_cast<int>(MessageType::Var), vars);
        std::cout << "worker " << world.rank() << " receive variables" << std::endl;
        for (auto var : vars) std::cout << var << std::endl;
    }
    
    return 0;
}

