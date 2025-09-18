#include "EnvironmentVisual.hpp"
#include <iostream>
#include "NeuralNetwork.hpp"
#include "helper.hpp"
// testing genomes and neural networks 
int main (){

    Genome genome ; 
    
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::OUTPUT); 
    genome.addNode(NodeType::OUTPUT); 

    genome.addConnection(0,6,getRandNum(-2,2), true,0); 
    genome.addConnection(0,7,getRandNum(-2,2), true,1); 
    genome.addConnection(1,6,getRandNum(-2,2), true,2); 
    genome.addConnection(1,7,getRandNum(-2,2), true,3); 
    genome.addConnection(2,6,getRandNum(-2,2), true,4); 
    genome.addConnection(2,7,getRandNum(-2,2), true,5); 
    genome.addConnection(3,6,getRandNum(-2,2), true,6); 
    genome.addConnection(3,7,getRandNum(-2,2), true,7); 
    genome.addConnection(4,6,getRandNum(-2,2), true,8); 
    genome.addConnection(4,7,getRandNum(-2,2), true,9); 
    genome.addConnection(5,6,getRandNum(-2,2), true,10); 
    genome.addConnection(5,7,getRandNum(-2,2), true,11); 
    



    std::cout<<"Nodes:\t" ; 
    for(Node node: genome.nodes){
        std::cout<< node.id << ":"<< (NodeType) node.type<<", "; 
    }
    std::cout<<"\nConnections: " <<std::endl;
    for(Connection conn : genome.connections){
        std::cout<<conn.inId << "->"<<conn.outId<<"\t w = "<<conn.weight<<"\t"
        << (conn.isEnabled ? "enabled" : "disabled") <<"\t"<<"innv: "<< conn.innvNum <<std::endl; 
    } 

    NeuralNetwork network(genome); 
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
