// this is the sfml container for the rocket object
// so we can disable visuals if need be 
#pragma once 
#include <SFML/Graphics.hpp>
#include "Rocket.hpp"

class RocketVisual{
    private:
    Rocket& rocket;          // reference to underlying rocket
    sf::RectangleShape rect; // visual representation

public:
    // constructor
    RocketVisual(Rocket& r) 
        : rocket(r), rect(sf::Vector2f(10.f, 40.f)) 
    {
        rect.setFillColor(sf::Color::Blue);
        rect.setOrigin(sf::Vector2f(rect.getSize().x / 2, rect.getSize().y / 2));
    }

    void draw(sf::RenderWindow& window) {
        rect.setPosition(sf::Vector2f(rocket.getX(),rocket.getY() )); 
        rect.setRotation(sf::degrees(rocket.getRotation())); 
        window.draw(rect);
    }
}; 