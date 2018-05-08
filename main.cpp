//
// Created by zq32 on 5/7/18.
//

#include "FactorGraph.h"


int main() {

    const int master_rank = 0;
    const std::vector<int> workers_rank = {1, 2};
    size_t num_samples = 10000;

    FactorGraph graph;
    graph.generate_BDC_instance();
    graph.resample(num_samples, master_rank, workers_rank);

}
