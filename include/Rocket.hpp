#pragma once 
#include <cmath>
#include <Eigen/Dense>
#define M_PI 3.14159265358979323846
class Rocket{
public:         
    static constexpr float MASS = 1;
    static constexpr int BASE = 15;
    static constexpr int HEIGHT= 30;
    int rotation=0; // 0-360 , start at 0
    float thrust=0.0f; // 0-1
    

    //vectors will be stored using eigen
    Eigen::Vector2f pos; 
    Eigen::Vector2f vel;  

    static constexpr float GRAVITY = 35; //350; 
    static constexpr float MAX_THRUST =360;// 700; 
    static constexpr float MAX_VEL = 600; 
    
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
    
    // returns postitions rockets vertices as a mat of 3 row vectors CCW 
    Eigen::Matrix<float, 2,3> getVertices(); 
    
    // Setters
    void setRotation(int r);
    void setThrust(float t);
    
    // for triangle starting at 0 deg; CCW
    Eigen::Matrix<float, 2, 3> vLocal{
        {HEIGHT / 2.0f, -HEIGHT / 2.0f, -HEIGHT / 2.0f},
        {0, BASE / 2.0f, -BASE / 2.0f}}; // local vertice calcs for triangle, private so don't recalc everytime
};