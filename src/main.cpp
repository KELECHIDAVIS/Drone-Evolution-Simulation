#include "EnvironmentVisual.hpp"
#include <iostream>

int main()
{
    

    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
    window.setFramerateLimit(144);
    
    // init env
    Environment env (window.getSize().x, window.getSize().y); 
    EnvironmentVisual envVis(env.rocket, env.target, env); 
    env.addThrust(.3333); 

    // vertex checkers for testing 
    sf::CircleShape checkers[3]; 
    for(int i =0; i<3; i++){
        checkers[i] = sf::CircleShape(4);
        checkers[i].setFillColor(sf::Color::Green); 
        checkers[i].setOrigin(sf::Vector2f(2,2));  
    } 
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
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::D){
                    env.rotate(-15)  ; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::S){
                    env.addThrust(-.1)  ; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::W){
                    env.addThrust(.1)  ; 
                }
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        env.update(deltaTime); 

        // update checkers 
        Eigen::Matrix<float, 2,3> verts  = env.getRocketVertices(); 
        for (int i = 0; i< 3 ; i++){
            checkers[i].setPosition(sf::Vector2f(verts.col(i)(0), window.getSize().y - verts.col(i)(1))); 
        }
        window.clear();
        
        // draw
        envVis.draw(window); 
        for (int i = 0; i< 3 ; i++){
            window.draw(checkers[i]); 
        }
        window.display();
    }
}
