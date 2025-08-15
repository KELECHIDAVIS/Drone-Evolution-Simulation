// this is the sfml container for the rocket object
// so we can disable visuals if need be 
#pragma once 
#include <SFML/Graphics.hpp>
#include "Rocket.hpp"
#include <iostream> 
class RocketVisual{
    private:
    Rocket& rocket;          // reference to underlying rocket
    sf::ConvexShape body; // visual representation

public:
    // constructor
    RocketVisual(Rocket& r) 
        : rocket(r)  // rockets start on their side and are then rotated up 
    {
        body.setPointCount(3); 
        // Set points (tip at top, base at bottom)
        body.setPoint(0, sf::Vector2f(0.f, 0.f));          // top tip
        body.setPoint(1, sf::Vector2f(-rocket.getBase()/2.f, rocket.getHeight())); // bottom-left
        body.setPoint(2, sf::Vector2f(rocket.getBase()/2.f,rocket.getHeight()));  // bottom-right
        body.setFillColor(sf::Color::Blue);    
        // Center the origin
        sf::FloatRect bounds = body.getLocalBounds();
        body.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.f,
                   bounds.position.y + bounds.size.y / 2.f)) ; 

       
    }

    void draw(sf::RenderWindow& window) ; 
    void rotate(int deg); 
    void setThrust(float thrust); 
    Rocket& getRocket(); 
}; 