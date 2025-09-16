#include "EnvironmentVisual.hpp"
#include <iostream>


int main (){
    return 0 ; 
}


// int main()
// {
    

//     sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920, 1080}), "Drone Evo");
//     window.setFramerateLimit(144);
    
    
//     sf::Clock clock; 
//     while (window.isOpen())
//     {
//         while (const std::optional event = window.pollEvent())
//         {
//             if (event->is<sf::Event::Closed>())
//             {
//                 window.close();
//             }else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
//             {
//                 if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
//                     window.close();
//             }
//         }

//         // update 


//         window.clear();
//         // draw

//         window.display();
//     }
// }
