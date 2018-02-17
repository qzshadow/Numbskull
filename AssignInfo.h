//
// Created by zq32 on 2/16/18.
//

#ifndef NUMBSKULL_ASSIGNINFO_H
#define NUMBSKULL_ASSIGNINFO_H

#include <boost/serialization/access.hpp>
#include <boost/mpi/datatype.hpp>

class AssignInfo {
public:
    size_t var_start_idx;
    size_t num_variables;
    size_t fac_start_idx;
    size_t num_factors;

    AssignInfo(size_t vi, size_t nv, size_t fi, size_t nf) :
            var_start_idx(vi), num_variables(nv), fac_start_idx(fi), num_factors(nf) {}

    AssignInfo() = default;

    AssignInfo &operator=(AssignInfo other) {
        var_start_idx = other.var_start_idx;
        num_variables = other.num_variables;
        fac_start_idx = other.fac_start_idx;
        num_factors = other.num_factors;
        return *this;
    }

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & var_start_idx;
        ar & num_variables;
        ar & fac_start_idx;
        ar & num_factors;
    }

};
BOOST_IS_BITWISE_SERIALIZABLE(AssignInfo)


#endif //NUMBSKULL_ASSIGNINFO_H
