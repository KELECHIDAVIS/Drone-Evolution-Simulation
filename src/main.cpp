#include "EnvironmentVisual.hpp"
#include "helper.hpp"
#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <cassert>



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
