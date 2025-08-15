#include "Environment.hpp"

// updates the rocket and checks if it collided with the target
void Environment::update(float deltaTime){
    rocket.update(deltaTime); 
    checkCollision(); 
}

//CAN FURTHER OPTIMIZE THIS 
// ONLY CHECK COLLISION IF THEy ARE CLOSE (euclidean distance is is <= diameter of target + height of rocket)
// SAT between 
//USING LINEAR ALG TO MAKE FAST
void Environment::checkCollision()
{
    // first make sure they are close enough to check 
    float dx = target.x - rocket.getX(); 
    float dy = target.y - rocket.getY();
    
    float dist = sqrt(dx*dx +dy*dy); 

    if(dist <= target.radius*2+ rocket.getHeight())
        return; 
    
    // else check intersection
    bool intersection = separateAxis(); 
}

bool Environment::separateAxis(){
    // get normals from triangle
    // vertices are ccw
    
    return true; 
}