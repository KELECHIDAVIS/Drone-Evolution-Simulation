#pragma once 
#include <cmath>
#include <Eigen/Dense>
#define M_PI 3.14159265358979323846
class Rocket{
public:         
    const float mass = 1;  
    const float base = 40; 
    const float height = 70; 
    int rotation=0; // 0-360 , start at 0
    float thrust=0.0f; // 0-1
    

    //vectors will be stored using eigen
    Eigen::Vector2f pos; 
    Eigen::Vector2f vel;  

    static constexpr float GRAVITY = 30; //350; 
    static constexpr float MAX_THRUST =180;// 700; 
    static constexpr float MAX_VEL = 500; // TODO: tune ; used for normalizing velocity inputs
    
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
        {height/2.0f, -height/2.0f, -height/2.0f},
        {0, base/2.0f, -base/2.0f}
    }; // local vertice calcs for triangle, private so don't recalc everytime 
};