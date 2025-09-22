#pragma once 
#include <random>

#include <utility>
// in some global utility or singleton RNG
std::mt19937 rng(std::random_device{}());

// pair hash is needed to make pairs work in unorderd pairs within the NEAT RUNNER Class
struct pair_hash {
    std::size_t operator()(const std::pair<int,int>& p) const noexcept {
        // Simple mixing function (XOR with a shift)
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

// random number between range 
double getRandNum ( double lower , double upper ){
    std::uniform_real_distribution<double> dist(lower, upper);
    return dist(rng);
}

