/**
 * Variable class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_VARIABLE_H
#define NUMBSKULL_VARIABLE_H

#include <iostream>
#include <boost/serialization/access.hpp>
#include <boost/mpi/datatype.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <string>
#include <vector>

class Variable {
public:
    Variable() = default;

    Variable(size_t _vid, std::string _assign, int _value, std::vector<size_t> _factors);

    Variable &operator=(Variable other);

    friend std::ostream &operator<<(std::ostream &os, const Variable &var);

    // variable id
    size_t vid = SIZE_MAX;
    // variable value
    int value;
    // assign info (e.g. "B0", variable type 'B', assign to machine 0)
    std::string assign;
    // factor id associated with this variable
    std::vector<size_t> factors;

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
        ar & vid;
        ar & value;
        ar & assign;
        ar & factors;
    }

};
BOOST_IS_BITWISE_SERIALIZABLE(Variable)


#endif //NUMBSKULL_VARIABLE_H
