#ifndef NUMBSKULL_FACTOR_H
#define NUMBSKULL_FACTOR_H

#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>


class Factor {
public:

    Factor(size_t _fid, const std::string &_assign, const std::vector<size_t> &_variables,
           const std::string &_type, double _weight);

    Factor &operator=(Factor other);
    friend std::ostream&operator<<(std::ostream&, const Factor& fac);

    Factor();

    friend class boost::serialization::access;

    template<class Archieve>
    void serialize(Archieve &ar, const unsigned int version_num) {
        ar & fid;
        ar & assign;
        ar & variables;
        ar & type;
        ar & weight;
    }

public:
    size_t fid = SIZE_MAX;
    std::string assign;
    std::vector<size_t> variables;
    std::string type; // operator
    double weight;
};

BOOST_IS_BITWISE_SERIALIZABLE(Factor)


#endif //NUMBSKULL_FACTOR_H
