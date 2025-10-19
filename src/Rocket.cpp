#include "Rocket.hpp"

int Rocket::sfmlToMathAngle(int sfAngleDeg)
{
    return (90.f - sfAngleDeg) ; 
}

int Rocket::mathToSfmlAngle(int mathAngleDeg)
{
    return 90.f - (mathAngleDeg);
}

void Rocket::update(float deltaTime)
{
    // every frame update the rocket's pos based on gravity and thrust
    
    // calc accel 
    float thrustForce = thrust * MAX_THRUST; 
    
    
    float angleRad = degToRad(rotation);

    // Thrust in direction of rocket
    float a_thrust_x = (thrustForce * cos(angleRad)) / MASS;
    float a_thrust_y = (thrustForce * sin(angleRad) ) / MASS;


    // Total acceleration
    float a_x = a_thrust_x;
    float a_y = a_thrust_y -GRAVITY; // gravity is alr in accel

    // calc final vels  
    float v_f_x = vel(0) +a_x*deltaTime;
    float v_f_y = vel(1) +a_y*deltaTime; 
    
    // calc final positions
    pos(0)+= (v_f_x + vel(0))*deltaTime/2.0; 
    pos(1) += (v_f_y + vel(1))*deltaTime/2.0; 

    // set init vels to finals 
    vel(0) = v_f_x; 
    vel(1)= v_f_y;
    
}

// SHOULD BE IN MATH DEGS NOT SFML
void Rocket::rotate(int deg)
{
    setRotation(getRotation() + deg); 
}

float Rocket::degToRad(int deg)
{
    return deg* (M_PI/180.0);  
}


// --- Rotation ---
int Rocket::getRotation() const { return rotation; }
void Rocket::setRotation(int r) {
    if(r < 0) rotation = r+360;
    else if(r > 360) rotation = r-360;
    else rotation = r;
}

// --- Thrust ---
float Rocket::getThrust() const { return thrust; }


// RETURNS SFML COORDINATES FOR VERTICES 
Eigen::Matrix<float, 2, 3> Rocket::getVertices()
{
    // create rotation matrix 
    float cosTheta = cos(degToRad(rotation));  
    float sinTheta= sin(degToRad(rotation));  
    Eigen::Matrix2f R {
        {cosTheta, -sinTheta},
        {sinTheta, cosTheta},
    }; 
    Eigen::Matrix<float, 2,3> vWorld = R*vLocal;
    vWorld.colwise() += pos; // translate to world pos 

    // // flip for sfml 
    // vWorld.row(1) = -vWorld.row(1);
    return vWorld; 
}
void Rocket::setThrust(float t)
{
    if(t < 0.0f) thrust = 0.0f;
    else if(t > 1.0f) thrust = 1.0f;
    else thrust = t;
}
