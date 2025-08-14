#pragma once 
#include <cmath>
class Rocket{
private:         
    float mass = 1;  
    int rotation=0; // 0-360 
    float thrust=0.0f; // 0-1
    int x, y; 
    float xVel=0, yVel=0; 

public:
    static constexpr float GRAVITY = 10; 
    static constexpr float MAX_THRUST = 20; 
    Rocket(int x, int y )
    : x(x), y(y) {}

    
    void update(float deltaTime);

    float degToRad(int deg ); 
    // getters and setters
    // Getters
    float getMass() const;
    int getRotation() const;
    float getThrust() const;
    int getX() const;
    int getY() const;
    float getXVel() const;
    float getYVel() const;

    // Setters
    void setMass(float m);
    void setRotation(int r);
    void setThrust(float t);
    void setX(int newX);
    void setY(int newY);
    void setXVel(float vx);
    void setYVel(float vy);
};