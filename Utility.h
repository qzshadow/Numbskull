//
// The utility function used in this project
//

#ifndef CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H
#define CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H


class Utility {
public:
    static int randomChoice(std::vector<int> values, std::vector<float> energy);

    //static int randomChoice(std::map<int, float> val_energy);

    static int randomChoice(std::vector<float> energy_vec);
};


#endif //CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H
