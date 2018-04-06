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

    virtual size_t get_vid() {};

    virtual void set_value(int value) {};

    virtual void set_factor_vec(std::vector<Factor *>)= 0;
};

class BinaryVariable : public Variable {
public:
    BinaryVariable() = default;

    BinaryVariable(size_t vid, int value, float prior_energy);

    BinaryVariable(size_t vid, int value, float prior_energy, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

    inline int get_value() override { return _value; }

    inline void set_value(int value) override { _value = value; }

    inline size_t get_vid() override { return _vid; }

    inline void set_factor_vec(std::vector<Factor *> fac_vec) override {
        _factor_ptr_vec = std::make_unique<std::vector<Factor *>>(fac_vec);
    }


private:
    size_t _vid = SIZE_MAX;
    int _value = 0;
    float _prior_energy = 0.0;
    std::unique_ptr<std::vector<Factor *>> _factor_ptr_vec;// = nullptr;
};

class MultinomialVariable : public Variable {
public:
    MultinomialVariable() = default;

    MultinomialVariable(size_t vid, int value, float prior_energy);

    MultinomialVariable(size_t vid, int value, float prior_energy, std::vector<Factor *> factor_ptr_vec);

    void resample() override;

    inline int get_value() override { return _value; };

    inline void set_value(int value) override { _value = value; }

    inline size_t get_vid() override { return _vid; }

    inline void set_factor_vec(std::vector<Factor *> fac_vec) override {
        _factor_ptr_vec = std::make_unique<std::vector<Factor *>>(fac_vec);
    }


private:
    size_t _vid = SIZE_MAX;
    unsigned int _domain_size = 0;
    int _value = 0;
    float _prior_energy = 0.0;
    std::unique_ptr<std::vector<Factor *>> _factor_ptr_vec;// = nullptr;
};


#endif //NUMBSKULL_VARIABLE_H
