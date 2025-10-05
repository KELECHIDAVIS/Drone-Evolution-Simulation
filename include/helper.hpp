#pragma once 
#include <random>

#include <utility>


// pair hash is needed to make pairs work in unorderd pairs within the NEAT RUNNER Class
struct pair_hash {
    std::size_t operator()(const std::pair<int,int>& p) const noexcept {
        // Simple mixing function (XOR with a shift)
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

inline std::mt19937& getRNG() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

inline double getRandNum(double lower, double upper) {
    std::uniform_real_distribution<double> dist(lower, upper);
    return dist(getRNG());
}
