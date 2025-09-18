#include "EnvironmentVisual.hpp"
#include "helper.hpp"
#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <iostream>
#include <cmath>
#include <Eigen/Dense>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

// Test 1: Identity network
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
    input << 1.0, -1.0; // raw inputs

    Eigen::VectorXd output = net.feedForward(input);

    double expected0 = 1.0 / (1.0 + std::exp(-4.9 * 1.0));
    double expected1 = 1.0 / (1.0 + std::exp(-4.9 * -1.0));

    std::cout << "[Identity Test]\n";
    std::cout << "Expected: " << expected0 << ", " << expected1 << "\n";
    std::cout << "Got:      " << output(0) << ", " << output(1) << "\n\n";
}

// Test 2: Single hidden node
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

    std::cout << "[Hidden Node Test]\n";
    std::cout << "Expected: " << expected << "\n";
    std::cout << "Got:      " << output(0) << "\n\n";
}

// Test 3: Bias node
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

    double expected = 1.0 / (1.0 + std::exp(-4.9 * 1.0));

    std::cout << "[Bias Test]\n";
    std::cout << "Expected: " << expected << "\n";
    std::cout << "Got:      " << output(0) << "\n\n";
}

// Test 4: Multi-path
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

    std::cout << "[Multi-path Test]\n";
    std::cout << "Expected: " << expected << "\n";
    std::cout << "Got:      " << output(0) << "\n\n";
}

int main() {
    testIdentityNetwork();
    testHiddenNode();
    testBias();
    testMultiPath();
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
