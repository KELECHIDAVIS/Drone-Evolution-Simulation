#pragma once 
#include <random>
#include "helper.hpp"
#include <Eigen/Dense>

class Target{
public:

    Eigen::Vector2f pos; 
    float radius=30; 

    // targets are seeded with score so that they have the same random position based on score
    Target( int windowWidth , int windowHeight){
        respawn(windowWidth,windowHeight); 
    }

    void respawn( int width, int height) {
        std::uniform_real_distribution<float> distX(0, width);
        std::uniform_real_distribution<float> distY(0, height);
        pos = {distX(rng), distY(rng)};
    }

}; 