#include "RocketVisual.hpp"

int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "CMake SFML Project");
    window.setFramerateLimit(144);
    
    Rocket rocket(950,540); 
    RocketVisual rv(rocket); 

    sf::Clock clock; 
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // update 
        float deltaTime = clock.restart().asSeconds(); 
        rocket.update(deltaTime); 
        window.clear();
        // draw
        rv.draw(window); 
        window.display();
    }
}
