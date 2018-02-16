#include "Variable.h"

Variable::Variable(size_t _vid, std::string _assign, double _value, std::vector<size_t> _factors) {
    vid = _vid;
    assign = std::move(_assign);
    factors = std::move(_factors);
    value = _value;
}

Variable &Variable::operator=(Variable other) {
    vid = other.vid;
    assign = other.assign;
    factors = other.factors;
    value = other.value;
    return *this;
}

std::ostream &operator<<(std::ostream &os,  const Variable &var) {
    os<<var.vid<<std::ends<<var.assign<<std::ends<<"["<<std::ends;
    for (auto fac : var.factors) os<<fac<<std::ends;
    os<<"]"<<std::ends<<var.value<<std::endl;
    return os;
}

Variable::Variable() = default;
