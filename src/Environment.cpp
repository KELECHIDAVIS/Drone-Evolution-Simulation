#include "Environment.hpp"
#include <iostream> 
// updates the rocket and checks if it collided with the target
bool Environment::update(float deltaTime){
    rocket.update(deltaTime); 


    if( checkCollision()){ 
        score++;
        target.respawn(windowWidth, windowHeight); 
    }

    // if rocket outside of bounds it's not alive
    if (rocket.pos(0) < 0 || rocket.pos(0) > windowWidth ||
        rocket.pos(1) < 0 || rocket.pos(1) > windowHeight)
    {
        return false; // Freeze
    }

    return true; 
}
void Environment::reset() {
    rocket.setThrust(0); 
    rocket.setRotation(0);
    rocket.pos=Eigen::Vector2f(windowWidth/2.0, windowHeight/2.0); 
    rocket.vel=Eigen::Vector2f(0, 0); 
    
    
    target.respawn(windowWidth, windowHeight); 
    score = 0;
}
//CAN FURTHER OPTIMIZE THIS 
// ONLY CHECK COLLISION IF THEy ARE CLOSE (euclidean distance is is <= diameter of target + height of rocket)
// SAT between 
//USING LINEAR ALG TO MAKE FAST
bool Environment::checkCollision()
{
    // first make sure they are close enough to check 
    float dx = target.pos(0) - rocket.pos(0); 
    float dy = target.pos(1) - rocket.pos(1);
    
    float dist = sqrt(dx*dx +dy*dy);

    float rocketRadius = std::sqrt(
        (Rocket::BASE / 2.f) * (Rocket::BASE / 2.f) +
        (Rocket::HEIGHT / 2.f) * (Rocket::HEIGHT / 2.f));

    if (dist > Target::RADIUS + rocketRadius)
        return false; // No way they're touching
    

    
    // else check intersection
    bool intersection = separateAxis(); 

    return intersection; 
}

void Environment::addThrust(float thrust)
{
    rocket.setThrust (thrust+rocket.getThrust())    ; 
}

void Environment::rotate(float deg)
{
    rocket.setRotation(rocket.getRotation()+deg); 
}

Eigen::Matrix<float, 2, 3> Environment::getRocketVertices()
{
    return rocket.getVertices();
}

float projectScalar(Eigen::Vector2f n, Eigen::Vector2f a)
{
    float dotProduct = a.dot(n);
    float denom = n.dot(n);
    return dotProduct / denom; // Returns a scalar, not a vector
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
        float max = -std::numeric_limits<float>::infinity();
        float min = std::numeric_limits<float>::infinity();

        for (int i = 0; i < 3; i++)
        {
            float proj = projectScalar(n, vertices.col(i));
            max = std::max(max, proj);
            min = std::min(min, proj);
        }

        // Project target center
        float targetProj = projectScalar(n, target.pos);
        float circleMax = targetProj + Target::RADIUS;
        float circleMin = targetProj - Target::RADIUS;

        if (max < circleMin || min > circleMax)
        {
            return false; // Gap found, no collision
        }
    }

    return true; 
}

