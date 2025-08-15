#pragma once 
#include <cmath>
#include <Eigen/Dense>

class Rocket{
public:         
    const float mass = 1;  
    const float base = 40; 
    const float height = 70; 
    int rotation=180; // 0-360 , should point upwards initially 
    float thrust=0.0f; // 0-1
    

    //vectors will be stored using eigen
    Eigen::Vector2d pos; 
    Eigen::Vector2d vel;  

    static constexpr float GRAVITY = 30; 
    static constexpr float MAX_THRUST = 150; 
    
    Rocket(float x, float y ){
        pos<<x , y; 
        vel<<0,0; 
    }

    // SFML -> math angle (DEG)
    static int sfmlToMathAngle(int sfAngleDeg); 
    // Math angle (DEG) -> SFML
    static int  mathToSfmlAngle(int mathAngleRad) ; 
    
    void update(float deltaTime);
    void rotate(int deg); // add amount of rotation 
    float degToRad(int deg ); 
    
    // getters and setters
    // Getters
    int getRotation() const;
    float getThrust() const;
    
    
    // Setters
    void setRotation(int r);
    void setThrust(float t);
    
};