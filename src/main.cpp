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
    std::cout<<"Rocket Local pos: \n"<< rv.getRocket().vLocal<<std::endl<<std::endl; ; 
    std::cout<<"Rocket World pos: \n"<< rv.getRocket().getVertices()<<std::endl<<"Center Pos: \n"<<rv.getRocket().pos; 
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
                    std::cout<<"Rocket World pos: \n"<< rv.getRocket().getVertices()<<std::endl<<"Center Pos: \n"<<rv.getRocket().pos; 
                }
                if(keyPressed->scancode == sf::Keyboard::Scancode::D){
                    rv.rotate(-15); 
                    std::cout<<"Rocket World pos: \n"<< rv.getRocket().getVertices()<<std::endl<<"Center Pos: \n"<<rv.getRocket().pos; 
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
