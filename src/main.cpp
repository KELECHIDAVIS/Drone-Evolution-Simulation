#include "EnvironmentVisual.hpp"
#include <iostream>

int main()
{
    

    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
    window.setFramerateLimit(144);
    
    // init env
    Environment env (window.getSize().x, window.getSize().y); 
    EnvironmentVisual envVis(env.rocket, env.target, env); 
    env.addThrust(.1); 

    // second env 
    Environment env2 (window.getSize().x, window.getSize().y); 
    EnvironmentVisual env2Vis(env2.rocket, env2.target, env2); 
    env2.addThrust(.7); 
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
                    env.rotate(15)  ; 
                    env2.rotate(15)  ; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::D){
                    env.rotate(-15)  ; 
                    env2.rotate(-15)  ; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::S){
                    env.addThrust(-.1)  ; 
                    env2.addThrust(-.1)  ; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::W){
                    env.addThrust(.1)  ; 
                    env2.addThrust(.1)  ; 
                }
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        env.update(deltaTime); 
        env2.update(deltaTime); 

        
        window.clear();
        
        // draw
        envVis.draw(window); 
        env2Vis.draw(window); 
        
        window.display();
    }
}
