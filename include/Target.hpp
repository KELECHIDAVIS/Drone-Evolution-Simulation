#pragma once 
#include <random>
#include "helper.hpp"
#include <Eigen/Dense>

class Target{
public:

    Eigen::Vector2f pos; 
    static constexpr float RADIUS=8; 

    // targets are seeded with score so that they have the same random position based on score
    Target( int windowWidth , int windowHeight){
        respawn(windowWidth,windowHeight); 
    }

    void respawn( int width, int height) {

        // shouldn't be on the border 
        float randWidth = getRandNum(width*.1 , width*.9);
        float randHeight = getRandNum(height * .1, height * .9);
        pos = {randWidth,randHeight };
    }

}; 