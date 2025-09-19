#include "EnvironmentVisual.hpp"
#include "helper.hpp"
#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <cassert>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

void testIdentityNetwork() {
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::INPUT);   // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 2, 1.0, true, 0);
    genome.addConnection(1, 3, 1.0, true, 1);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(2);
    input << 1.0, -1.0;
    Eigen::VectorXd output = net.feedForward(input);

    auto sigmoid = [](double x) { return 1.0 / (1.0 + std::exp(-4.9 * x)); };

    assert(approxEqual(output(0), sigmoid(1.0)));
    assert(approxEqual(output(1), sigmoid(-1.0)));

    std::cout << "✅ Identity Network test passed\n";
}

void testHiddenNode() {
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::INPUT);   // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 2, 1.0, true, 0);
    genome.addConnection(1, 2, 1.0, true, 1);
    genome.addConnection(2, 3, 1.0, true, 2);
 
    NeuralNetwork net(genome);

    Eigen::VectorXd input(2);
    input << 1.0, 2.0;
    Eigen::VectorXd output = net.feedForward(input);

    auto sigmoid = [](double x) { return 1.0 / (1.0 + std::exp(-4.9 * x)); };

    double h1 = sigmoid(1.0 + 2.0);
    double expected = sigmoid(h1);

    assert(approxEqual(output(0), expected));

    std::cout << "✅ Hidden Node test passed\n";
}

void testBias() {
    Genome genome;
    genome.addNode(NodeType::INPUT);  // id = 0
    genome.addNode(NodeType::BIAS);   // id = 1
    genome.addNode(NodeType::OUTPUT); // id = 2

    genome.addConnection(1, 2, 1.0, true, 0);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 123.0; // ignored
    Eigen::VectorXd output = net.feedForward(input);

    auto sigmoid = [](double x) { return 1.0 / (1.0 + std::exp(-4.9 * x)); };
    double expected = sigmoid(1.0);

    assert(approxEqual(output(0), expected));

    std::cout << "✅ Bias Node test passed\n";
}

void testMultiPath() {
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 1, 1.0, true, 0);
    genome.addConnection(0, 2, 2.0, true, 1);
    genome.addConnection(1, 3, 1.0, true, 2);
    genome.addConnection(2, 3, 1.0, true, 3);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;
    Eigen::VectorXd output = net.feedForward(input);

    auto sigmoid = [](double x) { return 1.0 / (1.0 + std::exp(-4.9 * x)); };

    double h1 = sigmoid(1.0);
    double h2 = sigmoid(2.0);
    double expected = sigmoid(h1 + h2);

    assert(approxEqual(output(0), expected));

    std::cout << "✅ Multi-path test passed\n";
}

int main() {
    testIdentityNetwork();
    testHiddenNode();
    testBias();
    testMultiPath();

    std::cout << "🎉 All tests passed!\n";
    return 0;
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
