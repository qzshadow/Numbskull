/*
 * Factor type class
 * use enum class to represent factor types
 * only support EQU now
 */
#ifndef MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H
#define MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H

#include <iostream>

enum class FactorType {
    EQU
};
std::istream& operator >> (std::istream& is, FactorType& factor_type);
std::ostream& operator << (std::ostream& os, const FactorType& factor_type);


#endif //MAKE_BUILD_DEBUG_NUMBSKULL_FACTORTYPE_H
