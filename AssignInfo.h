/*
 * AssignInfo class
 * This class contains detailed assign info on each machine
 * See comments of class members for details.
 */
#ifndef NUMBSKULL_ASSIGNINFO_H
#define NUMBSKULL_ASSIGNINFO_H

#include <boost/serialization/access.hpp>
#include <boost/mpi/datatype.hpp>
#include <ostream>

class AssignInfo {
public:
    // the variable start idx on this machine, e.g., var_start_idx = 5, then the gloable vid of vars[0] is 0 + 5 = 5
    size_t var_start_idx;
    // how many variables stored on this machine, equals to vars.size()
    size_t num_variables;
    // the factor start idx on this machine
    size_t fac_start_idx;
    // how many factors stored on this machine, equals to facs.size()
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

    friend std::ostream &operator<<(std::ostream &os, const AssignInfo &info) {
        os << info.var_start_idx << std::ends << info.num_variables << std::ends << info.fac_start_idx << std::ends
           << info.num_factors << std::endl;
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
