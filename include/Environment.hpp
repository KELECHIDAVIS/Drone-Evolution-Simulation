#pragma once
#include "Target.hpp"
#include "Rocket.hpp"

class Environment{
public:
    Target target;
    Rocket rocket; 
    int score=0; 
    int windowWidth;
    int windowHeight;

    // init target and rocket 
    Environment(int width, int height)
    : windowWidth(width), windowHeight(height), target(width, height), rocket(width/2.0,height/2.0) {}


    void update(float deltaTime); 
    bool separateAxis(); 
    Eigen::Vector2f projection(Eigen::Vector2f n, Eigen::Vector2f a ); 
    bool checkCollision(); 
    void addThrust(float thrust);
    void rotate(float deg);  
    Eigen::Matrix<float, 2,3> getRocketVertices(); 

}; 