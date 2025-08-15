#include "EnvironmentVisual.hpp"

// rocket visuals
void RocketVisual::draw(sf::RenderWindow &window){
    // SINCE AXISES ARE FLIPPED IN SFML DRAWING IT WITH THIS IS MIND SO THAT MATH IS NORMAL
    body.setPosition(sf::Vector2f(rocket.pos(0),window.getSize().y - rocket.pos(1) )); 
    body.setRotation(sf::degrees(Rocket::mathToSfmlAngle((rocket.getRotation()))));
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

// target implementation

void TargetVisual::draw(sf::RenderWindow& window) {
    shape.setPosition(sf::Vector2f(target.pos(0), target.pos(1)));
    window.draw(shape);
}

// Environment impl
void EnvironmentVisual::draw(sf::RenderWindow& window){
    rocketVisual.draw(window); 
    targetVisual.draw(window); 
}