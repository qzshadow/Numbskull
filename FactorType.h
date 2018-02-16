//
// Created by ZHOU QIN on 2/13/18.
//

#ifndef MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H
#define MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H

#include <iostream>

enum class FactorType {
    EQU
};
std::istream& operator >> (std::istream& is, FactorType& factor_type);
std::ostream& operator << (std::ostream& os, const FactorType& factor_type);


#endif //MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H
