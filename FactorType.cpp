//
// Created by ZHOU QIN on 2/13/18.
//

#include "FactorType.h"

std::istream &operator>>(std::istream &is, FactorType &factor_type) {

    std::string type;
    if (is >> type) {
        if (type == "EQU") factor_type = FactorType::EQU;
        else {
            std::cerr << "Unrecognized Factor Type" << std::endl;
            exit(-1);
        }
    }
    return is;

}

std::ostream &operator<<(std::ostream &os, const FactorType &factor_type) {
    if (factor_type == FactorType::EQU) os<<"EQU"<<std::ends;
    else os<<"UNKNOWN"<<std::ends;
    return os;
}

