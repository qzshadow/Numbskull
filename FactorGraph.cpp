


#include "FactorGraph.h"


void FactorGraph::gibbs(size_t num_samples, int master_rank, std::vector<int> workers_rank) {
    std::unordered_map<size_t, std::array<int, 2>> counter;
    while (num_samples--) {
        if (world.rank() == master_rank) { // master
            // Master broadcasts its assignment of all B-key variables to the workers
            if (this->var_ptr_map.count("B")) {
                size_t B_var_size = this->var_ptr_map["B"].size();
                std::vector<int> B_val_vec(B_var_size);
                for (size_t i = 0; i < B_var_size; ++i)
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
    //for (auto &entry : counter) {
    //    for (int v = 0; v < 2; ++v)
    //        std::cout << "machine#" << world.rank() << " var: " << entry.first << " value: " << v << " count: "
    //                  << entry.second[v] << std::endl;
    //}
}

void FactorGraph::gen_BDC_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker,
                                   size_t var_num_per_factor) {
    if (world.rank() == 0) {
        std::vector<Edge *> B_var_edges;
        for (auto vid = 0; vid < var_num_on_master; ++vid) {
            auto *var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            auto *edge = new IdentityEdge(vid, var, "U");
            B_var_edges.push_back(edge);
        }
        for (auto w = 0; w < worker_nums; ++w) {
            std::string worker_assign = "D" + std::to_string(w + 1);
            for (auto fac_idx = 0; fac_idx < factor_num_per_worker; ++fac_idx) {
                auto pfid = w * worker_nums + fac_idx;
                auto *partial_fac = new PatialAndFactor(pfid, B_var_edges, 1.0, worker_assign);
                partial_factor_ptr_map[worker_assign].push_back(partial_fac);
                for (auto &var : var_ptr_map["B"])
                    var->add_factor(partial_fac);
            }
        }
    } else {
        //int worker_rank = world.rank();
        std::vector<Edge *> B_var_edges;
        size_t vid = 0;
        for (; vid < var_num_on_master; ++vid) {
            auto *var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            auto *edge = new IdentityEdge(vid, var, "U");
            B_var_edges.push_back(edge);
        }
        for (auto fid = 0; fid < factor_num_per_worker; fid++) {
            auto *factor = new AndFactor(fid, B_var_edges, 1.0, "D");
            for (; vid < var_num_on_master + (fid + 1) * var_num_per_factor; vid++) {
                auto *var = new BinaryVariable(vid, 0, 0.0, "C");
                var_ptr_map["C"].push_back(var);
                var->add_factor(factor);
                auto *edge = new IdentityEdge(vid, var, "U");
                factor->add_edge(edge);
            }
            factor_ptr_map["D"].push_back(factor);
        }
    }
}


void FactorGraph::gen_AGC_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker,
                                   size_t var_num_per_factor) {
    if (world.rank() == 0) {
        std::vector<Edge *> A_var_edges;
        for (auto vid = 0; vid < var_num_on_master; ++vid) {
            auto *var = new BinaryVariable(vid, 0, 0.0, "A");
            var_ptr_map["A"].push_back(var);
            auto *edge = new IdentityEdge(vid, var, "U");
            A_var_edges.push_back(edge);
        }
        for (auto w = 0; w < worker_nums; ++w) {
            std::string worker_assign = "G" + std::to_string(w + 1);
            for (auto fac_idx = 0; fac_idx < factor_num_per_worker; ++fac_idx) {
                auto pfid = w * worker_nums + fac_idx;
                auto *partial_fac = new PatialAndFactor(pfid, A_var_edges, 1.0, worker_assign);
                partial_factor_ptr_map[worker_assign].push_back(partial_fac);
                for (auto &var : var_ptr_map["A"])
                    var->add_factor(partial_fac);
            }
        }
    } else {
        size_t vid = 0;
        for (auto fid = 0; fid < factor_num_per_worker; fid++) {
            auto *factor = new PatialAndFactor(fid, {}, 1.0, "G");
            for (; vid < (fid + 1) * var_num_per_factor; vid++) {
                auto *var = new BinaryVariable(vid, 0, 0.0, "C");
                var_ptr_map["C"].push_back(var);
                var->add_factor(factor);
                auto *edge = new IdentityEdge(vid, var, "U");
                factor->add_edge(edge);
            }
            partial_factor_ptr_map["G"].push_back(factor);
        }
    }
}

void FactorGraph::gen_AED_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor) {
    if (world.rank() == 0) {
        size_t vid = 0;
        std::vector<Edge*> A_var_edges;
        for (; vid < var_num_on_master; ++vid) {
            auto* var = new BinaryVariable(vid, 0, 0.0, "A");
            var_ptr_map["A"].push_back(var);
            auto* edge = new IdentityEdge(vid, var, "U");
            A_var_edges.push_back(edge);
        }
        for (auto w = 0; w < worker_nums; ++w) {
            std::string factor_assign = "E" + std::to_string(w + 1);
            std::string var_assign = "D" + std::to_string(w + 1);
            for (auto fid = 0; fid < factor_num_per_worker; ++fid) {
                auto* factor = new AndFactor(fid, A_var_edges, 1.0, factor_assign);
                for (; vid < var_num_on_master + w * factor_num_per_worker * var_num_per_factor + (fid + 1) * var_num_per_factor; vid++) {
                    auto* var = new BinaryVariable(vid, 0, 0.0, var_assign);
                    var_ptr_map[var_assign].push_back(var);
                    var->add_factor(factor);
                    auto* edge = new IdentityEdge(vid, var, "U");
                    factor->add_edge(edge);
                }
                factor_ptr_map[factor_assign].push_back(factor);
                for (auto& var : var_ptr_map["A"]) {
                    var->add_factor(factor);
                }
            }
        }
    } else {
        size_t vid = 0;
        for (auto fid = 0; fid < factor_num_per_worker; ++fid) {
            auto* factor = new PatialAndFactor(fid, {}, 1.0, "E");
            for (; vid < (fid + 1) * var_num_per_factor; ++vid) {
                auto var = new BinaryVariable(vid, 0, 0.0, "D");
                var_ptr_map["D"].push_back(var);
                var->add_factor(factor);
                auto* edge = new IdentityEdge(vid, var, "U");
                factor->add_edge(edge);
            }
            partial_factor_ptr_map["E"].push_back(factor);
        }
    }
}

void FactorGraph::gen_BFD_instance(size_t worker_nums, size_t var_num_on_master, size_t factor_num_per_worker, size_t var_num_per_factor) {
    if (world.rank() == 0) {
        size_t vid = 0;
        std::vector<Edge*> B_var_edges;
        for (; vid < var_num_on_master; ++vid) {
            auto* var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            auto* edge = new IdentityEdge(vid, var, "U");
            B_var_edges.push_back(edge);
        }
        for (auto w = 0; w < worker_nums; ++w) {
            std::string factor_assign = "F" + std::to_string(w + 1);
            std::string var_assign = "D" + std::to_string(w + 1);
            for (auto fid = 0; fid < factor_num_per_worker; ++fid) {
                auto * factor = new AndFactor(fid, B_var_edges, 1.0, factor_assign);
                for (; vid < var_num_on_master + w * factor_num_per_worker * var_num_per_factor + (fid + 1) * var_num_per_factor; vid++) {
                    auto *var = new BinaryVariable(vid, 0, 0.0, var_assign);
                    var_ptr_map[var_assign].push_back(var);
                    var->add_factor(factor);
                    auto* edge = new IdentityEdge(vid, var, "U");
                    factor->add_edge(edge);
                }
                factor_ptr_map[factor_assign].push_back(factor);
                for (auto& var : var_ptr_map["B"]) {
                    var->add_factor(factor);
                }
            }
        }
    } else {
        size_t vid = 0;
        std::vector<Edge*> B_var_edges;
        for (; vid < var_num_on_master; vid++) {
            auto* var = new BinaryVariable(vid, 0, 0.0, "B");
            var_ptr_map["B"].push_back(var);
            auto* edge = new IdentityEdge(vid, var, "U");
            B_var_edges.push_back(edge);
        }
        for (auto fid = 0; fid < factor_num_per_worker; ++fid) {
            auto* factor = new AndFactor(fid, B_var_edges, 1.0, "F");
            for (; vid < var_num_on_master + (fid + 1) * var_num_per_factor; ++vid) {
                auto* var = new BinaryVariable(vid, 0, 0.0, "D");
                var_ptr_map["D"].push_back(var);
                var->add_factor(factor);
                auto* edge = new IdentityEdge(vid, var, "U");
                factor->add_edge(edge);
            }
            factor_ptr_map["F"].push_back(factor);
        }
    }
}

