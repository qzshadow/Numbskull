/**
 * Variable class
 * See comments of class members for details
 */
#ifndef NUMBSKULL_VARIABLE_H
#define NUMBSKULL_VARIABLE_H


#include <vector>
#include <cstddef>
class Variable {
public:
    // friend std::ostream &operator<<(std::ostream &os, const Variable &var);
    Variable() = default;
    virtual void resample() = 0;

    virtual bool get_value_bool() {};

    virtual int get_value_int() {};
};

class BinaryVariable : public Variable {
public:
    BinaryVariable() = default;
    BinaryVariable(bool val, float prior_energy, std::vector<size_t> fac_id_vec);

    void resample() override;
    inline bool get_value_bool() override {return _value;};
    //int get_value_int();
private:
    bool _value = 0;
    float _prior_energy = 0.0;
    std::vector<size_t> _fac_id_vec;
};

class MultinomialVariable : public Variable {
public:
    MultinomialVariable() = default;
    MultinomialVariable(bool val, float prior_energy, std::vector<size_t> fac_id_vec);

    void resample() override;
    inline int get_value_int() override {return _value;};
    //bool get_value_bool();
private:
    int domain_size = 0;
    bool _value = 0;
    float _prior_energy = 0.0;
    std::vector<size_t> _fac_id_vec;
};


#endif //NUMBSKULL_VARIABLE_H
