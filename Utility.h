//
// The utility function used in this project
//

#ifndef CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H
#define CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H


class Utility {
public:
    static int randomChoice(std::vector<int> values, std::vector<double> probs);

    static int randomChoice(std::map<int, double> val_probs);
};


#endif //CMAKE_BUILD_DEBUG_NUMBSKULL_UTILITY_H
