#pragma once
#include <SFML/Graphics.hpp>
#include "Rocket.hpp"
#include "Target.hpp"
#include "Environment.hpp"
class RocketVisual {
private:
    Rocket& rocket;
    sf::ConvexShape body;

public:
    RocketVisual(Rocket& r):rocket(r){
        body.setPointCount(3); 
        // Set points (tip at top, base at bottom)
        body.setPoint(0, sf::Vector2f(0.f, 0.f));          // top tip
        body.setPoint(1, sf::Vector2f(-rocket.base/2.f, rocket.height)); // bottom-left
        body.setPoint(2, sf::Vector2f(rocket.base/2.f,rocket.height));  // bottom-right
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
    void update();
};

class TargetVisual {
private:
    Target& target;
    sf::CircleShape shape;

public:
    TargetVisual(Target& t):target(t){
        shape = sf::CircleShape(target.radius); 
        shape.setFillColor(sf::Color::Red);    
        // Center the origin
       
        shape.setOrigin(sf::Vector2f(target.radius,
                   target.radius) ); 
    }

    void draw(sf::RenderWindow& window);
    void update();
};

class EnvironmentVisual {
private:
    TargetVisual targetVisual; 
    RocketVisual rocketVisual; 
    Environment& env; 
public:
    EnvironmentVisual(Rocket& rocket, Target& target , Environment &e ) : rocketVisual(rocket), targetVisual(target), env(e) {}
    void draw(sf::RenderWindow& window);
};
