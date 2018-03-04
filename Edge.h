//
// Created by zq32 on 3/4/18.
//

#ifndef CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
#define CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H

#include <vector>
#include <climits>
#include <string>
#include <boost/mpi/datatype.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

class Edge {
public:
    virtual int transform(int value) = 0;

    Edge() = default;
    Edge &operator=(const Edge& other) = default;
    Edge(size_t _eid, const std::string& _assign, const std::vector<size_t>& _var_ids):
            eid(_eid), assign(_assign), var_ids(_var_ids){};

    //edge id
    size_t eid = SIZE_MAX;
    // edge assign
    std::string assign;
    // variable ids associated to this edge
    std::vector<size_t> var_ids;


private:
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version_num) {
        ar & eid;
        ar & assign;
        ar & var_ids;
    }


};

BOOST_IS_BITWISE_SERIALIZABLE(Edge)

class IdentityEdge : public Edge {
public:
    int transform(int value) override {
        return value;
    }

private:
    friend class boost::serialization::access;
    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version_num) {
        ar & boost::serialization::base_object<Edge>(*this);
    }
};
BOOST_IS_BITWISE_SERIALIZABLE(IdentityEdge)


class NegEdge : public Edge {
public:
    int transform(int value) override {
        return -value;
    }
};

#endif //CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
