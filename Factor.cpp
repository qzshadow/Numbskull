#include "Factor.h"

Factor::Factor(size_t _fid,
               const std::string &_assign,
               const std::vector<size_t> &_variables,
               const std::string &_type,
               double _weight) {
    fid = _fid,
            assign = _assign;
    variables = _variables,
            type = _type;
    weight = _weight;
}

Factor &Factor::operator=(Factor other) {
    fid = other.fid;
    assign = other.assign;
    variables = other.variables;
    type = other.type;
    weight = other.weight;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Factor &fac) {
    os << fac.fid << std::ends << fac.assign << std::ends << "[";
    for (auto vid : fac.variables) os << vid << std::ends;
    os << "]" << std::ends << fac.type << std::ends << fac.weight << std::endl;
    return os;
}

Factor::Factor() = default;


