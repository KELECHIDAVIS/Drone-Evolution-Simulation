#include "RocketVisual.hpp"
#include <iostream>
int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
    window.setFramerateLimit(144);
    
    Rocket rocket(950,540); 
    RocketVisual rv(rocket); 
    //rv.setThrust(1.0f);
    sf::Clock clock; 
    std::cout<<"Math To SFML: "<<Rocket::mathToSfmlAngle(rv.getRocket().getRotation())<<std::endl; 
    std::cout<<"Straight from rocket: "<<rv.getRocket().getRotation()<<std::endl; 
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

                if(keyPressed->scancode == sf::Keyboard::Scancode::Space){
                    rv.rotate(10); 
                    std::cout<<"Math To SFML: "<<Rocket::mathToSfmlAngle(rv.getRocket().getRotation())<<std::endl; 
                    std::cout<<"Straight from rocket: "<<rv.getRocket().getRotation()<<std::endl; 
                }
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        //rocket.update(deltaTime); 
        
        window.clear();
        // draw
        rv.draw(window); 
        window.display();
    }
}
