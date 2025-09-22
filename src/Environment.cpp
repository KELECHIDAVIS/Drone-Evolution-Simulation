#include "Environment.hpp"

// updates the rocket and checks if it collided with the target
void Environment::update(float deltaTime){
    rocket.update(deltaTime); 

    if( checkCollision()){ 
        score++; 
        target.respawn(windowWidth, windowHeight); 
    }
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
        (rocket.base / 2.f) * (rocket.base / 2.f) +
        (rocket.height / 2.f) * (rocket.height / 2.f)
    );

    if (dist > target.radius + rocketRadius)
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
        // project each vertice on the line 
        // then get the max and min mag on that line 
        float max= INT_MIN , min = INT_MAX; 
        for (int i =0 ; i< 3; i++ ){
            Eigen::Vector2f proj = projection(n, vertices.col(i)); // MIGHT HAVE TO NORMALIZE IF DOESN'T WORK 
            float norm = proj.norm(); 
            max= std::max(max,norm); 
            min= std::min(min,norm); 
        }
        
        // find circle's  
        Eigen::Vector2f proj = projection(n, target.pos); 
        float norm = proj.norm(); 
        // subtract and add radius for it's max and min 
        float circleMax = target.radius +norm; 
        float circleMin = -target.radius + norm ; 
    
        // check for collision on this axis 
        if(max < circleMin){
            return false; // gap
        }else if (min > circleMax){
            return false; 
        }// else they are touching on this axis 
    }

    return true; 
}
// return the projection of a onto n 
Eigen::Vector2f Environment::projection(Eigen::Vector2f n, Eigen::Vector2f a)
{
    float dotProduct= a.dot(n); 
    float denom = n.dot(n); 
    return (dotProduct/denom) * n ;
}
