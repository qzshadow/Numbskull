/*
 * Factor class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_FACTOR_H
#define NUMBSKULL_FACTOR_H

#include "Edge.h"
#include <cstddef>
#include <vector>

class Factor {
public:
    virtual float eval() = 0;
};

class AndFactor : public Factor{
public:
    AndFactor() = default;

    AndFactor(std::vector<Edge *> edge_ptr_vec, float weight);
    float eval() override;

private:
    std::unique_ptr<std::vector<Edge *>> _edge_ptr_vec;
    float _weight = 1.0;

};


#endif //NUMBSKULL_FACTOR_H
