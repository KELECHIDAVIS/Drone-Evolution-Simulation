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
    float dx = target.pos(0) - rocket.pos(0); 
    float dy = target.pos(1) - rocket.pos(1);
    
    float dist = sqrt(dx*dx +dy*dy); 

    if(dist <= target.radius*2+ rocket.height)
        return; 
    
    // else check intersection
    bool intersection = separateAxis(); 
}

bool Environment::separateAxis(){
    // get normals from triangle
    // vertices are ccw
    Eigen::Matrix<float,2,3> vertices = rocket.getVertices(); 

    // get edge segments from vertices 
    Eigen::Vector2f e1= vertices.col(1) - vertices.col(0);  
    Eigen::Vector2f e2= vertices.col(2) - vertices.col(1);  
    Eigen::Vector2f e3= vertices.col(0) - vertices.col(2);
    
    // calc axis from center or target to center of triang
    Eigen::Vector2f e4(target.pos(0)- rocket.pos(0), target.pos(1)-rocket.pos(1)); 

    // list of axises
    Eigen::Vector2f axes[4]={
        Eigen::Vector2f(-e1.y(),e1.x() ),
        Eigen::Vector2f(-e2.y(),e2.x() ),
        Eigen::Vector2f(-e3.y(),e3.x() ),
        Eigen::Vector2f(e4.x(), e4.y()), 
    }; 

    // for each axis, project and check if they are gaps 
    for (Eigen::Vector2f n : axes){
        
    }

    return true; 
}