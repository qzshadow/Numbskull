/**
 * Variable class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_VARIABLE_H
#define NUMBSKULL_VARIABLE_H


#include <vector>
#include <cstddef>
#include <memory>

//#include "Factor.h"
class Factor; // Forward declare Factor class to break out the circular include of Factor Edge and Variable
class Variable {
public:
    // friend std::ostream &operator<<(std::ostream &os, const Variable &var);
    Variable() = default;
    virtual void resample() = 0;

    virtual int get_value() {};
};

class BinaryVariable : public Variable {
public:
    BinaryVariable() = default;

    BinaryVariable(int value, float prior_energy, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

    inline int get_value() override { return _value; };
private:
    int _value = 0;
    float _prior_energy = 0.0;
    std::unique_ptr<std::vector<Factor *>> _factor_ptr_vec;// = nullptr;
};

class MultinomialVariable : public Variable {
public:
    MultinomialVariable() = default;

    MultinomialVariable(int value, float prior_energy, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

    inline int get_value() override { return _value; };
private:
    unsigned int _domain_size = 0;
    int _value = 0;
    float _prior_energy = 0.0;
    std::unique_ptr<std::vector<Factor *>> _factor_ptr_vec;// = nullptr;
};


#endif //NUMBSKULL_VARIABLE_H
