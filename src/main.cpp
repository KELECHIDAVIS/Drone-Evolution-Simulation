#include "EnvironmentVisual.hpp"
#include <iostream>

int main()
{
    

    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
    window.setFramerateLimit(144);
    
    // init env
    Environment env (window.getSize().x, window.getSize().y); 
    EnvironmentVisual envVis(env.rocket, env.target, env); 

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
                    
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::D){
                    
                }
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        env.update(deltaTime); 

        window.clear();
        
        // draw
        envVis.draw(window); 
        window.display();
    }
}
