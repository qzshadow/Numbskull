//
// The utility function used in this project
//

#ifndef NUMBSKULL_UTILITY_H
#define NUMBSKULL_UTILITY_H


class Utility {
public:
    static int randomChoice(std::vector<int> values, std::vector<float> energy);

    //static int randomChoice(std::map<int, float> val_energy);

    static int randomChoice(std::vector<float> energy_vec);
};


#endif //NUMBSKULL_UTILITY_H
