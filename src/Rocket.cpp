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
    float a_thrust_x = (thrustForce * cos(angleRad)) / mass;
    float a_thrust_y = (thrustForce * sin(angleRad)) / mass;

    // Gravity only along y
    float a_gravity_y = -GRAVITY;  // positive up in math coordinates

    // Total acceleration
    float a_x = a_thrust_x;
    float a_y = a_thrust_y + a_gravity_y;

    // calc final vels  
    float v_f_y = vel(1) +a_y*deltaTime; 
    float v_f_x = vel(0) +a_x*deltaTime;
    
    // calc final positions
    pos(0)+= (v_f_x + vel(0))*deltaTime/2.0; 
    pos(1) += (v_f_y + vel(1))*deltaTime/2.0; 

    // set init vels to finals 
    vel(1)= v_f_y;
    vel(0) = v_f_x; 
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
    return vWorld; 
}
void Rocket::setThrust(float t)
{
    if(t < 0.0f) thrust = 0.0f;
    else if(t > 1.0f) thrust = 1.0f;
    else thrust = t;
}
