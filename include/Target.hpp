#pragma once 
#include <random>
#include <Eigen/Dense>

class Target{
public:

    Eigen::Vector2f pos; 
    float radius=30; 

    // targets are seeded with score so that they have the same random position based on score
    Target(int score, int windowWidth , int windowHeight){
        std::mt19937 rng(score);
        std::uniform_real_distribution<float> distX(windowWidth * 0.1f, windowWidth * 0.9f);
        std::uniform_real_distribution<float> distY(windowHeight * 0.1f, windowHeight * 0.9f);

        
        // we want the target to be within the visible area 
        pos(0) = distX(rng); 
        pos(1)= distY(rng); 
        
    }
}; 