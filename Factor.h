/*
 * Factor class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_FACTOR_H
#define NUMBSKULL_FACTOR_H

#include "Edge.h"
#include <cstddef>
#include <vector>
#include <unordered_set>

class Factor {
public:
    Factor() = default;

    Factor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight);

    Factor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign);

    virtual ~Factor() = default;


    virtual float eval() = 0;

    virtual float partial_eval(std::unordered_set<std::string>) {};

    size_t get_fid() { return _fid; }

    inline void add_edge(Edge*edge) {_edge_ptr_vec->push_back(edge);}

protected:
    size_t _fid = SIZE_MAX;
    std::unique_ptr<std::vector<Edge *>> _edge_ptr_vec = std::make_unique<std::vector<Edge*>>();
    float _weight = 1.0;
    std::string _assign;
};

class PatialFactor : virtual public Factor {
public:
    PatialFactor() = default;

    PatialFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign);

    PatialFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight);

    //virtual float eval() = 0;

    void set_partial_val(float partial_val) { _patial_val = partial_val; }

protected:
    float _patial_val = 0.0;

};


class AndFactor : virtual public Factor{
public:
    AndFactor() = default;

    AndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign);

    float eval() override;

    float partial_eval(std::unordered_set<std::string>) override;

protected:
AndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight);
private:
    //float eval() override;

};

class PatialAndFactor : public PatialFactor, public AndFactor {
public:
    PatialAndFactor() = default;
    PatialAndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight, std::string assign);
    float eval() override;

private:
    PatialAndFactor(size_t fid, std::vector<Edge *> edge_ptr_vec, float weight);
};


#endif //NUMBSKULL_FACTOR_H
