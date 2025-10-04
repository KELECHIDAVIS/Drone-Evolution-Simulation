#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <cassert>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

// Helper to compute expected activation based on type
double computeActivation(double x, ActivationType type) {
    if (type == TANH) {
        return std::tanh(2.5 * x);
    } else {
        return 1.0 / (1.0 + std::exp(-4.9 * x));
    }
}

// Helper to map activation to output range
double mapToOutput(double activation, ActivationType type) {
    if (type == TANH) {
        return (activation + 1.0) / 2.0;  // (-1,1) -> (0,1)
    } else {
        return activation;  // Already (0,1)
    }
}

void testIdentityNetwork(ActivationType activationType) {
    std::cout << "\n✅ Testing Identity Network (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::INPUT);   // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 2, 1.0, true, 0, false);
    genome.addConnection(1, 3, 1.0, true, 1, false);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(2);
    input << 1.0, -1.0;
    Eigen::VectorXd output = net.feedForward(input);

    double expected0 = mapToOutput(computeActivation(1.0, activationType), activationType);
    double expected1 = mapToOutput(computeActivation(-1.0, activationType), activationType);

    assert(approxEqual(output(0), expected0));
    assert(approxEqual(output(1), expected1));

    std::cout << "  Output 0: " << output(0) << " (expected: " << expected0 << ")\n";
    std::cout << "  Output 1: " << output(1) << " (expected: " << expected1 << ")\n";
    std::cout << "  ✓ Identity Network test passed\n";
}

void testHiddenNode(ActivationType activationType) {
    std::cout << "\n✅ Testing Hidden Node (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::INPUT);   // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 2, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(2, 3, 1.0, true, 2, false);
 
    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(2);
    input << 1.0, 2.0;
    Eigen::VectorXd output = net.feedForward(input);

    double h1 = computeActivation(1.0 + 2.0, activationType);
    double expected = mapToOutput(computeActivation(h1, activationType), activationType);

    assert(approxEqual(output(0), expected));
    
    std::cout << "  Hidden activation: " << h1 << "\n";
    std::cout << "  Output: " << output(0) << " (expected: " << expected << ")\n";
    std::cout << "  ✓ Hidden Node test passed\n";
}

void testBias(ActivationType activationType) {
    std::cout << "\n✅ Testing Bias (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);  // id = 0
    genome.addNode(NodeType::BIAS);   // id = 1
    genome.addNode(NodeType::OUTPUT); // id = 2

    genome.addConnection(1, 2, 1.0, true, 0, false);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(1);
    input << 123.0; // ignored
    Eigen::VectorXd output = net.feedForward(input);

    double expected = mapToOutput(computeActivation(1.0, activationType), activationType);

    assert(approxEqual(output(0), expected));
    
    std::cout << "  Output: " << output(0) << " (expected: " << expected << ")\n";
    std::cout << "  ✓ Bias test passed\n";
}

void testMultiPath(ActivationType activationType) {
    std::cout << "\n✅ Testing Multi-path (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(0, 2, 2.0, true, 1, false);
    genome.addConnection(1, 3, 1.0, true, 2, false);
    genome.addConnection(2, 3, 1.0, true, 3, false);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(1);
    input << 1.0;
    Eigen::VectorXd output = net.feedForward(input);

    double h1 = computeActivation(1.0, activationType);
    double h2 = computeActivation(2.0, activationType);
    double expected = mapToOutput(computeActivation(h1 + h2, activationType), activationType);

    assert(approxEqual(output(0), expected));
    
    std::cout << "  Hidden 1: " << h1 << ", Hidden 2: " << h2 << "\n";
    std::cout << "  Output: " << output(0) << " (expected: " << expected << ")\n";
    std::cout << "  ✓ Multi-path test passed\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   NEURAL NETWORK TESTS (BOTH MODES)   \n";
    std::cout << "========================================\n";

    // Test with SIGMOID
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "         TESTING WITH SIGMOID          \n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    testIdentityNetwork(SIGMOID);
    testHiddenNode(SIGMOID);
    testBias(SIGMOID);
    testMultiPath(SIGMOID);

    // Test with TANH
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "           TESTING WITH TANH           \n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    testIdentityNetwork(TANH);
    testHiddenNode(TANH);
    testBias(TANH);
    testMultiPath(TANH);

    std::cout << "\n========================================\n";
    std::cout << "🎉 ALL TESTS PASSED FOR BOTH MODES!\n";
    std::cout << "========================================\n";
    
    return 0;
}