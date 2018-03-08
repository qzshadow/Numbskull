//
// Created by zq32 on 3/4/18.
//

#ifndef CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
#define CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H

#include <cstddef>
#include <cstdint>
#include "Variable.h"

class Edge {
public:
    virtual float transform() = 0;
};

class NegateEdge : public Edge {
public:
    float transform() override;

    NegateEdge(Variable *var, size_t eid);
private:
    Variable *_var = nullptr;
    size_t _eid = SIZE_MAX;
    //float _edge_val = 0.0;

};

class IdentityEdge : public Edge {
public:
    float transform() override;

private:
    Variable *_var = nullptr;
    size_t _eid = SIZE_MAX;
    //float _edge_val = 0.0


};

#endif //CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
