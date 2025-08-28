#pragma once 
#include <random>
#include <iostream>

#include <Eigen/Dense>

class Target{
public:

    Eigen::Vector2f pos; 
    float radius=30; 

    // targets are seeded with score so that they have the same random position based on score
    Target(int score, int windowWidth , int windowHeight){
        std::mt19937 rng(score);

        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        float xNorm = dist(rng);
        float yNorm = dist(rng);

        pos(0) = windowWidth * 0.1f + xNorm * (windowWidth * 0.8f);
        pos(1) = windowHeight * 0.1f + yNorm * (windowHeight * 0.8f);

        std::cout << "Score " << score << " → pos = (" << pos(0) << ", " << pos(1) << ")\n";

    }
}; 