#pragma once 
#include <cstdlib>

class Target{
public:

    Eigen::Vector2f pos; 
    float radius=30; 

    // targets are seeded with score so that they have the same random position based on score
    Target(int score, int windowWidth , int windowHeight){
        srand(score); 
        
        // we want the target to be within the visible area 
        pos(0) = windowWidth*.1f + (rand() % ((int)(windowWidth*.9f) - (int)(windowWidth*.1f  + 1)));
        pos(1)= windowHeight*.1f + (rand() % ((int)(windowHeight*.9f) - (int)(windowHeight*.1f )+ 1));
        
    }
}; 