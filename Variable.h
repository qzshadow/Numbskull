#ifndef NUMBSKULL_VARIABLE_H
#define NUMBSKULL_VARIABLE_H

#include <vector>
#include <string>
#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

class Variable {
public:
    Variable();

    Variable(size_t _vid, std::string _assign, double _value, std::vector<size_t> _factors);

    Variable &operator=(Variable other);

    friend std::ostream &operator<<(std::ostream &os, const Variable &var);

    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version_num) {
        ar & vid;
        ar & assign;
        ar & value;
        ar & factors;
    }


    size_t vid = SIZE_MAX; // variable id
    std::string assign;
    double value;
    std::vector<size_t> factors;


};
BOOST_IS_MPI_DATATYPE(Variable)


#endif //NUMBSKULL_VARIABLE_H
