#pragma once
#include "Target.hpp"
#include "Rocket.hpp"

class Environment{
public:
    Target target;
    Rocket& rocket; 
    int score=0; 

    void update(float deltaTime); 
    void checkCollision(); 

}; 