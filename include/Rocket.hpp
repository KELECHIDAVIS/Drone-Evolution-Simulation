#pragma once 
#include <cmath>
class Rocket{
private:         
    const float mass = 1;  
    int rotation=180; // 0-360 , should point upwards initially 
    float thrust=0.0f; // 0-1
    float x, y; 
    float xVel=0, yVel=0; 
    const float base = 40, height = 70; 

public:
    static constexpr float GRAVITY = 30; 
    static constexpr float MAX_THRUST = 150; 
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
    float getBase() const ; 
    float getHeight() const; 
    // Setters
    
    void setRotation(int r);
    void setThrust(float t);
    void setX(float newX);
    void setY(float newY);
    void setXVel(float vx);
    void setYVel(float vy);
    void setBase(float base)    ; 
    void setHeight(float height); 
};