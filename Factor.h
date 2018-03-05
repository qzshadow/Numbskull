/*
 * Factor class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_FACTOR_H
#define NUMBSKULL_FACTOR_H

#include "Variable.h"
#include <cstddef>
#include <vector>

class Factor {
public:
    virtual float eval() = 0;
};

class AndFactor : public Factor{
public:
    float eval() override;
private:
    std::vector<size_t> edge_id_vec;

};


#endif //NUMBSKULL_FACTOR_H
