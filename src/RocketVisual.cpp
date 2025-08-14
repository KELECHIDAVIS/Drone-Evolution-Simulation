#include "RocketVisual.hpp"

void RocketVisual::draw(sf::RenderWindow &window){
    // SINCE AXISES ARE FLIPPED IN SFML DRAWING IT WITH THIS IS MIND SO THAT MATH IS NORMAL
    body.setPosition(sf::Vector2f(this->rocket.getX(),window.getSize().y - this->rocket.getY() )); 
    body.setRotation(sf::degrees(Rocket::mathToSfmlAngle((this->rocket.getRotation()))));
    window.draw(body);
}

void RocketVisual::setThrust(float thrust)
{
    rocket.setThrust(thrust); 
}
void RocketVisual::rotate(int deg ){
    rocket.rotate(deg); 
}
Rocket& RocketVisual::getRocket(){
    return rocket; 
}
