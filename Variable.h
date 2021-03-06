/**
 * Variable class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_VARIABLE_H
#define NUMBSKULL_VARIABLE_H

#include <vector>
#include <cstddef>
#include <memory>
#include <string>

//#include "Factor.h"
class Factor; // Forward declare Factor class to break out the circular include of Factor, Edge and Variable
class Variable {
public:
    // friend std::ostream &operator<<(std::ostream &os, const Variable &var);
    virtual void resample() = 0;
    virtual ~Variable() = default;

    inline int get_value() {return _value;};

    inline std::string get_assign() {return _assign;}

    inline size_t get_vid() {return _vid;};

    inline void set_value(int value) {_value = value;};

    // add a factor to _factor_ptr_vec
    inline void add_factor(Factor* fac) {_factor_ptr_vec->push_back(fac);}

    // set _factor_ptr_vec
    // warning: any factors that exists in _factor_ptr_vec will be overwritten
    inline void set_factor_vec(std::vector<Factor *> fac_vec) {_factor_ptr_vec = std::make_unique<std::vector<Factor*>>(fac_vec);};

protected:
    size_t _vid = SIZE_MAX;
    int _value = 0;
    float _prior_energy = 0.0;
    std::string _assign;
    std::unique_ptr<std::vector<Factor *>> _factor_ptr_vec = std::make_unique<std::vector<Factor*>>();

    Variable() = default;
    Variable(size_t vid, int value, float prior_energy, std::string assign);
    Variable(size_t vid, int value, float prior_energy, std::string assign, std::vector<Factor *> factor_ptr_vec);

};

class BinaryVariable : public Variable {
public:
    BinaryVariable() = default;

    BinaryVariable(size_t vid, int value, float prior_energy, std::string assign);

    BinaryVariable(size_t vid, int value, float prior_energy, std::string assign, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

};

class MultinomialVariable : public Variable {
public:
    MultinomialVariable() = default;

    MultinomialVariable(size_t vid, int value, unsigned int domain_size, float prior_energy, std::string assign);

    MultinomialVariable(size_t vid, int value, unsigned int domain_size, float prior_energy, std::string assign, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

private:
    unsigned int _domain_size = 0;
};


#endif //NUMBSKULL_VARIABLE_H
