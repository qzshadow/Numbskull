//
// Created by zq32 on 3/4/18.
//

#ifndef NUMBSKULL_EDGE_H
#define NUMBSKULL_EDGE_H

#include <cstddef>
#include <cstdint>
#include "Variable.h"
#include <string>

class Edge {
public:
    Edge() = default;
    virtual ~Edge() = default;

    Edge(size_t eid, Variable *var, std::string assign);
    virtual float transform() = 0;

    Variable *get_var() const {
        return _var;
    }

protected:
    Variable *_var = nullptr;
    size_t _eid = SIZE_MAX;
    std::string _assign;
};

class NegateEdge : public Edge {
public:
    NegateEdge() = default;
    float transform() override;

    NegateEdge(size_t eid, Variable *var, std::string assign);

};

class IdentityEdge : public Edge {
public:
    IdentityEdge() = default;

    IdentityEdge(size_t eid, Variable *var, std::string assign);
    float transform() override;

};

#endif //NUMBSKULL_EDGE_H
