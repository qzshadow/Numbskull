//
// Created by zq32 on 3/4/18.
//

#ifndef CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
#define CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H

#include <cstddef>

class Edge {
public:
    virtual void transform() = 0;
};

class NegateEdge : public Edge {
public:
    void transform() override;
private:
    size_t var_id;

};

class IdentityEdge : public Edge {
public:
    void transform() override;

};

#endif //CMAKE_BUILD_DEBUG_NUMBSKULL_EDGE_H
