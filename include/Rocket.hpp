#pragma once 
#include <cmath>
class Rocket{
private:         
    float mass = 1;  
    int rotation=180; // 0-360 , should point upwards initially 
    float thrust=0.0f; // 0-1
    float x, y; 
    float xVel=0, yVel=0; 

public:
    static constexpr float GRAVITY = 30; 
    static constexpr float MAX_THRUST = 120; 
    // SFML -> math angle (DEG)
    static int sfmlToMathAngle(int sfAngleDeg); 
    // Math angle (DEG) -> SFML
    static int  mathToSfmlAngle(int mathAngleRad) ; 
    
    Rocket(float x, float y )
    : x(x), y(y) {

    }

    
    void update(float deltaTime);
    void rotate(int deg); // add amount of rotation 
    float degToRad(int deg ); 
    // getters and setters
    // Getters
    float getMass() const;
    int getRotation() const;
    float getThrust() const;
    float getX() const;
    float getY() const;
    float getXVel() const;
    float getYVel() const;

    // Setters
    
    void setMass(float m);
    void setRotation(int r);
    void setThrust(float t);
    void setX(float newX);
    void setY(float newY);
    void setXVel(float vx);
    void setYVel(float vy);
};