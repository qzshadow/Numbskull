


#include "FactorGraph.h"


void FactorGraph::gibbs(size_t num_samples, int master_rank, std::vector<int> workers_rank) {
    std::unordered_map<size_t, std::array<int, 2>> counter;
    while (num_samples--) {
        if (world.rank() == master_rank) { // master
            // Master broadcasts its assignment of all B-key variables to the workers
            if (this->var_ptr_map.count("B")) {
                size_t B_var_size = this->var_ptr_map["B"].size();
                std::vector<int> B_val_vec(B_var_size);
                for (size_t i = 0; i< B_var_size; ++i)
                    B_val_vec[i] = this->var_ptr_map["B"][i]->get_value();
                for (auto worker_rank : workers_rank)
                    world.send(worker_rank, static_cast<int>(MsgType::M_Send_B), B_val_vec);
            }
            // Master computes partial evaluation of E_i-key factors, and transmits them to worker i
            for (auto worker_rank : workers_rank) {
                std::string key = "E" + std::to_string(worker_rank);
                if (this->factor_ptr_map.count(key)) {
                    size_t E_pf_size = factor_ptr_map[key].size();
                    std::vector<float> E_pf_val_vec(E_pf_size);
                    for (auto i = 0; i < E_pf_size; ++i)
                        E_pf_val_vec[i] = factor_ptr_map[key][i]->partial_eval({"A", "B"});
                    world.send(worker_rank, static_cast<int>(MsgType::M_Send_P_E), E_pf_val_vec);
                }
            }
            // Master computes partial evaluation of G_i-key factors, and transmits them to worker i
            for (auto worker_rank : workers_rank) {
                std::string key = "G" + std::to_string(worker_rank);
                if (partial_factor_ptr_map.count(key)) {
                    size_t G_pf_size = partial_factor_ptr_map[key].size();
                    std::vector<float> G_pf_val_vec(G_pf_size);
                    for (auto i = 0; i < G_pf_size; ++i)
                        G_pf_val_vec[i] = partial_factor_ptr_map[key][i]->partial_eval({"A", "B"});
                    world.send(worker_rank, static_cast<int>(MsgType::M_Send_P_G), G_pf_val_vec);
                }
            }

            /********************* Master receive transmissions from workers ********************/
            // Master receive D_i variable from worker i, update its cache
            for (auto worker_rank : workers_rank) {
                std::string key = "D" + std::to_string(worker_rank);
                if (var_ptr_map.count(key)) {
                    std::vector<int> D_val_vec;
                    world.recv(worker_rank, static_cast<int>(MsgType::W_Send_D), D_val_vec);
                    assert(D_val_vec.size() == var_ptr_map[key].size());
                    for (auto i = 0; i < D_val_vec.size(); ++i)
                        var_ptr_map[key][i]->set_value(D_val_vec[i]);
                }
            }
            // Master receive D_i-key partial factor values from worker i, update its cache
            for (auto worker_rank : workers_rank) {
                std::string key = "D" + std::to_string(worker_rank);
                if (this->partial_factor_ptr_map.count(key)) {
                    std::vector<float> D_pf_val_vec;
                    world.recv(worker_rank, static_cast<int>(MsgType::W_Send_P_D), D_pf_val_vec);
                    assert(D_pf_val_vec.size() == this->partial_factor_ptr_map[key].size());
                    for (auto i = 0; i < D_pf_val_vec.size(); ++i)
                        this->partial_factor_ptr_map[key][i]->set_partial_val(D_pf_val_vec[i]);
                }

            }
            // Master receive G_i-key partial factor values from worker i, update its cache
            for (auto worker_rank : workers_rank) {
                std::string key = "G" + std::to_string(worker_rank);
                if (partial_factor_ptr_map.count(key)) {
                    std::vector<float> G_pf_vec;
                    world.recv(worker_rank, static_cast<int>(MsgType::W_Send_P_G), G_pf_vec);
                    assert(G_pf_vec.size() == partial_factor_ptr_map[key].size());
                    for (auto i = 0; i < G_pf_vec.size(); ++i)
                        partial_factor_ptr_map[key][i]->set_partial_val(G_pf_vec[i]);
                }

            }


            /******************** Master perform Gibbs sampling on the variables it owns *******/
            // Master runs a single pass of Gibbs sampling on the A variable it owns.
            if (this->var_ptr_map.count("A")) {
                for (auto &var_ptr : this->var_ptr_map["A"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            // Master runs a single pass of Gibbs sampling on the B variable it owns.
            if (this->var_ptr_map.count("B")) {
                for (auto &var_ptr : this->var_ptr_map["B"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            /********************************  Master End *******************************************/
        } else { // for worker machines
            /********************* Worker receive transmissions from master ***********************/
            // Worker receive B variable value vector from master, update cached assignment
            if (this->var_ptr_map.count("B")) {
                std::vector<int> B_val_vec;
                world.recv(master_rank, static_cast<int>(MsgType::M_Send_B), B_val_vec);
                for (auto i = 0; i < B_val_vec.size(); ++i)
                    this->var_ptr_map["B"][i]->set_value(B_val_vec[i]);
            }
            // Worker receive E partial factor values from master, update cached assignment
            if (partial_factor_ptr_map.count("E")) {
                std::vector<float> E_pf_val_vec;
                world.recv(master_rank, static_cast<int>(MsgType::M_Send_P_E), E_pf_val_vec);
                assert(E_pf_val_vec.size() == partial_factor_ptr_map["E"].size());
                for (auto i = 0; i < E_pf_val_vec.size(); ++i)
                    partial_factor_ptr_map["E"][i]->set_partial_val(E_pf_val_vec[i]);
            }
            // Worker receive G partial factor values from master, update cached assignment
            if (partial_factor_ptr_map.count("G")) {
                std::vector<float> G_pf_vec;
                world.recv(master_rank, static_cast<int>(MsgType::M_Send_P_G), G_pf_vec);
                assert(G_pf_vec.size() == partial_factor_ptr_map["G"].size());
                for (auto i = 0; i < G_pf_vec.size(); ++i)
                    partial_factor_ptr_map["G"][i]->set_partial_val(G_pf_vec[i]);
            }

            /*********** Worker performs Gibbs sampling on the variable it owns *************/
            // Worker runs a single pass of Gibbs sampling on the C variables it owns
            if (this->var_ptr_map.count("C")) {
                for (auto &var_ptr : this->var_ptr_map["C"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            // Worker runs a single pass of Gibbs sampling on the D variable it owns
            if (this->var_ptr_map.count("D")) {
                for (auto &var_ptr : this->var_ptr_map["D"]) {
                    var_ptr->resample();
                    counter[var_ptr->get_vid()][var_ptr->get_value()]++;
                }
            }
            /****************************** Worker transmit to master ****************************/
            // Worker transmits its assignment of all D_i-key variables to the master
            if (this->var_ptr_map.count("D")) {
                size_t D_var_vec_size = var_ptr_map["D"].size();
                std::vector<int> D_var_vec(D_var_vec_size);
                for (auto i = 0; i < D_var_vec_size; ++i)
                    D_var_vec[i] = var_ptr_map["D"][i]->get_value();
                world.send(master_rank, static_cast<int>(MsgType::W_Send_D), D_var_vec);
            }
            /************************ Worker eval patial factor values ***************************/
            // Worker computes partial evaluation of all D_i-key factors, and transmits them to master
            if (this->factor_ptr_map.count("D")) {
                size_t worker_D_pf_size = this->factor_ptr_map["D"].size();
                std::vector<float> D_pf_vec(worker_D_pf_size);
                for (auto i = 0; i < worker_D_pf_size; ++i)
                    D_pf_vec[i] = this->factor_ptr_map["D"][i]->partial_eval({"C", "D"});
                world.send(master_rank, static_cast<int>(MsgType::W_Send_P_D), D_pf_vec);
            }
            // Worker computes partial evaluation of all G_i-key factors, and transmits them to master
            if (partial_factor_ptr_map.count("G")) {
                size_t G_pf_size = partial_factor_ptr_map["G"].size();
                std::vector<float> G_pf_vec(G_pf_size);
                for (auto i = 0; i < G_pf_size; ++i) {
                    G_pf_vec[i] = partial_factor_ptr_map["G"][i]->partial_eval({"C", "D"});
                }
                world.send(master_rank, static_cast<int>(MsgType::W_Send_P_G), G_pf_vec);
            }

        }

    }
    /****************** statistics *****************************************************/
    for (auto &entry : counter) {
        for (int v = 0; v < 2; ++v)
            std::cout << "machine#"<< world.rank()<<" var: " << entry.first << " value: " << v << " count: " << entry.second[v] << std::endl;
    }
}
void FactorGraph::gen_BDC_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor) {
    if (world.rank() == 0) {
        for (auto vid = 0; vid < var_num_on_master; ++vid) {
            auto* var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            for (auto w = 0; w < worker_nums; ++w) {
                std::string worker_assign = "D" + std::to_string(w + 1);
                for (auto fac_idx = 0; fac_idx < factor_num_per_worker; ++fac_idx) {
                    auto pfid = w * worker_nums + fac_idx;
                    auto *edge = new IdentityEdge(pfid, var, "U");
                    auto *partial_fac = new PatialAndFactor(pfid, {edge}, 1.0, worker_assign);
                    partial_factor_ptr_map[worker_assign].push_back(partial_fac);
                    var->add_factor(partial_fac);
                }
            }
        }
    } else {
        int worker_rank = world.rank();
        std::vector<Edge* > B_var_edges;
        size_t vid = 0;
        for (; vid < var_num_on_master; ++vid) {
            auto* var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            auto* edge = new IdentityEdge(vid, var, "U");
            B_var_edges.push_back(edge);
        }
        for (auto fid = 0; fid < factor_num_per_worker; fid++) {
            auto* factor = new AndFactor(fid, B_var_edges, 1.0, "D");
            for (; vid < var_num_on_master + (fid + 1) * var_num_per_factor; vid++) {
                auto* var = new BinaryVariable(vid, 0, 0.0, "C");
                var_ptr_map["C"].push_back(var);
                var->add_factor(factor);
                auto* edge = new IdentityEdge(vid, var, "U");
                factor->add_edge(edge);
            }
            factor_ptr_map["D"].push_back(factor);
        }
    }
}
void FactorGraph::generate_BDC_instance() {
    if (world.rank() == 0) { // master
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *edge0 = new IdentityEdge(0, var0, "D1");
        auto *edge2 = new IdentityEdge(2, var0, "D2");
        var_ptr_map["B"] = {var0};
        auto *partial_fac0 = new PatialAndFactor(0, {edge0}, 1.0, "D1");
        auto *partial_fac1 = new PatialAndFactor(1, {edge2}, 1.0, "D2");
        partial_factor_ptr_map["D1"] = {partial_fac0};
        partial_factor_ptr_map["D2"] = {partial_fac1};
        var0->set_factor_vec({partial_fac0, partial_fac1});
    } else if (world.rank() == 1) { // worker0
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "C");
        auto *edge0 = new IdentityEdge(0, var0, "D");
        auto *edge1 = new IdentityEdge(1, var1, "C");
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0, "D");
        var1->set_factor_vec({factor0});
        var_ptr_map["C"] = {var1};
        var_ptr_map["B"] = {var0};
        factor_ptr_map["D"] = {factor0};

    } else if (world.rank() == 2) {// worker1
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "C");
        auto *edge2 = new IdentityEdge(2, var0, "D");
        auto *edge3 = new IdentityEdge(3, var2, "C");
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0, "D");
        var2->set_factor_vec({factor1});
        var_ptr_map["C"] = {var2};
        var_ptr_map["B"] = {var0};
        factor_ptr_map["D"] = {factor1};

    }
}

void FactorGraph::generate_AGC_instance() {
    if (world.rank() == 0) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "A");
        auto *edge0 = new IdentityEdge(0, var0, "A");
        auto *edge2 = new IdentityEdge(2, var0, "A");
        auto *pfactor0 = new PatialAndFactor(0, {edge0}, 1.0, "G1");
        auto *pfactor2 = new PatialAndFactor(2, {edge2}, 1.0, "G2");
        this->var_ptr_map["A"] = {var0};
        this->partial_factor_ptr_map["G1"] = {pfactor0};
        this->partial_factor_ptr_map["G2"] = {pfactor2};
        var0->set_factor_vec({pfactor0, pfactor2});
    } else if (world.rank() == 1) {
        auto *var1 = new BinaryVariable(1, 0, 0.0, "C");
        auto *edge1 = new IdentityEdge(1, var1, "C");
        auto *pfactor1 = new PatialAndFactor(1, {edge1}, 1.0, "G");
        this->var_ptr_map["C"] = {var1};
        this->partial_factor_ptr_map["G"] = {pfactor1};
        var1->set_factor_vec({pfactor1});
    } else if (world.rank() == 2) {
        auto *var2 = new BinaryVariable(2, 0, 0.0, "C");
        auto *edge3 = new IdentityEdge(3, var2, "C");
        auto *pfactor3 = new PatialAndFactor(3, {edge3}, 1.0, "G");
        this->var_ptr_map["C"] = {var2};
        this->partial_factor_ptr_map["G"] = {pfactor3};
        var2->set_factor_vec({pfactor3});
    }
}

void FactorGraph::generate_AED_instance() {
    if (world.rank() == 0) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "A");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "A");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D1");
        auto *var3 = new BinaryVariable(3, 0, 0.0, "D2");
        auto *edge0 = new IdentityEdge(0, var0, "A");
        auto *edge1 = new IdentityEdge(1, var1, "A");
        auto *edge2 = new IdentityEdge(1, var2, "A");
        auto *edge3 = new IdentityEdge(3, var0, "A");
        auto *edge4 = new IdentityEdge(4, var1, "A");
        auto *edge5 = new IdentityEdge(5, var3, "A");
        auto *factor0 = new AndFactor(0, {edge0, edge1, edge2}, 1.0, "E1");
        auto *factor1 = new AndFactor(1, {edge3, edge4, edge5}, 1.0, "E2");
        var0->set_factor_vec({factor0, factor1});
        var1->set_factor_vec({factor0, factor1});
        this->var_ptr_map["A"] = {var0, var1};
        this->var_ptr_map["D1"] = {var2};
        this->var_ptr_map["D2"] = {var3};
        this->factor_ptr_map["E1"] = {factor0};
        this->factor_ptr_map["E2"] = {factor1};

    } else if (world.rank() == 1) {
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D");
        auto *edge2 = new IdentityEdge(2, var2, "D");
        auto *pfactor0 = new PatialAndFactor(0, {edge2}, 1.0, "E");
        this->var_ptr_map["D"] = {var2};
        this->partial_factor_ptr_map["E"] = {pfactor0};
        var2->set_factor_vec({pfactor0});
    } else if (world.rank() == 2) {
        auto *var3 = new BinaryVariable(3, 0, 0.0, "D");
        auto *edge5 = new IdentityEdge(5, var3, "D");
        auto *pfactor1 = new PatialAndFactor(1, {edge5}, 1.0, "E");
        this->var_ptr_map["D"] = {var3};
        this->partial_factor_ptr_map["E"] = {pfactor1};
        var3->set_factor_vec({pfactor1});
    }
}

void FactorGraph::generate_BFD_instance() {
    if (world.rank() == 0) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "D1");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D2");
        this->var_ptr_map["B"] = {var0};
        this->var_ptr_map["D1"] = {var1};
        this->var_ptr_map["D2"] = {var2};
        auto *edge0 = new IdentityEdge(0, var0, "E1");
        auto *edge1 = new IdentityEdge(1, var1, "E1");
        auto *edge2 = new IdentityEdge(2, var0, "E2");
        auto *edge3 = new IdentityEdge(3, var2, "E2");
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0, "F1");
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0, "F2");
        var0->set_factor_vec({factor0, factor1});
    } else if (world.rank() == 1) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var1 = new BinaryVariable(1, 0, 0.0, "D");
        this->var_ptr_map["D"] = {var1};
        this->var_ptr_map["B"] = {var0};
        auto *edge0 = new IdentityEdge(0, var0, "E");
        auto *edge1 = new IdentityEdge(1, var1, "E");
        auto *factor0 = new AndFactor(0, {edge0, edge1}, 1.0, "F");
        var1->set_factor_vec({factor0});
    } else if (world.rank() == 2) {
        auto *var0 = new BinaryVariable(0, 0, 0.0, "B");
        auto *var2 = new BinaryVariable(2, 0, 0.0, "D");
        this->var_ptr_map["D"] = {var2};
        this->var_ptr_map["B"] = {var0};
        auto *edge2 = new IdentityEdge(2, var0, "E");
        auto *edge3 = new IdentityEdge(3, var2, "E");
        auto *factor1 = new AndFactor(1, {edge2, edge3}, 1.0, "F");
        var2->set_factor_vec({factor1});
    }
}


