#include "Factor.h"
#include <iostream>
int main() {
    auto *var1_ptr = new BinaryVariable(0, 0, 0.0);
    auto *var2_ptr = new BinaryVariable(1, 0, 0.0);
    auto *var3_ptr = new BinaryVariable(2, 0, 0.0);
    auto *edge1_ptr = new IdentityEdge(0, var1_ptr);
    auto *edge2_ptr = new IdentityEdge(1, var2_ptr);
    auto *edge3_ptr = new IdentityEdge(2, var1_ptr);
    auto *edge4_ptr = new IdentityEdge(3, var3_ptr);
    std::vector<Edge *> factor1_edge_ptr_vec = {edge1_ptr, edge2_ptr};
    auto *factor1_ptr = new AndFactor(factor1_edge_ptr_vec, 1.0);
    std::vector<Edge *> factor2_edge_ptr_vec = {edge3_ptr, edge4_ptr};
    auto *factor2_ptr = new AndFactor(factor2_edge_ptr_vec, 1.0);
    var1_ptr->set_factor_vec({factor1_ptr, factor2_ptr});
    var2_ptr->set_factor_vec({factor1_ptr});
    var3_ptr->set_factor_vec({factor2_ptr});
    std::vector<int> var1_count(2);
    std::vector<int> var2_count(2);
    std::vector<int> var3_count(2);

    for (int i = 0; i < 100; ++i) {
        var1_ptr->resample();
        var2_ptr->resample();
        var3_ptr->resample();

        var1_count[var1_ptr->get_value()]++;
        var2_count[var2_ptr->get_value()]++;
        var3_count[var3_ptr->get_value()]++;
    }
    std::cout << "var1=0: " << var1_count[0] << std::endl;
    std::cout << "var1=1: " << var1_count[1] << std::endl;
    std::cout << "var2=0: " << var2_count[0] << std::endl;
    std::cout << "var2=1: " << var2_count[1] << std::endl;
    std::cout << "var3=0: " << var3_count[0] << std::endl;
    std::cout << "var3=1: " << var3_count[1] << std::endl;

    return 0;
}

