#include "RocketVisual.hpp"
#include <iostream>
int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
    window.setFramerateLimit(144);
    
    Rocket rocket(950,540); 
    RocketVisual rv(rocket); 
    rv.setThrust(1.0f);

    sf::Clock clock; 
    
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();

                if(keyPressed->scancode == sf::Keyboard::Scancode::A){
                    rv.rotate(15); 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::D){
                    rv.rotate(-15); 
                }
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        std::cout<<"Rocket Rotation: "<< rocket.getRotation()<<std::endl; 
        rocket.update(deltaTime); 
        
        std::cout<<"Pos(x,y): "<< rv.getRocket().getX()<<" , "<<rv.getRocket().getY()<<std::endl; 
        std::cout<<"Vel(x,y): "<< rv.getRocket().getXVel()<<" , "<<rv.getRocket().getYVel()<<std::endl; 

        window.clear();
        // draw
        rv.draw(window); 
        window.display();
    }
}
