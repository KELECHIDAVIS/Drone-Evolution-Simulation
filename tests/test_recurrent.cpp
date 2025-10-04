#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <cassert>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

bool notApproxEqual(double a, double b, double minDiff = 1e-9) {
    return std::fabs(a - b) > minDiff;
}

// Helper to compute activation
double computeActivation(double x, ActivationType type) {
    if (type == TANH) {
        return std::tanh(2.5 * x);
    } else {
        return 1.0 / (1.0 + std::exp(-4.9 * x));
    }
}

// Helper to map to output
double mapToOutput(double activation, ActivationType type) {
    if (type == TANH) {
        return (activation + 1.0) / 2.0;
    } else {
        return activation;
    }
}

void testSimpleRecurrentLoop(ActivationType activationType) {
    std::cout << "\n🔄 Testing Simple Recurrent Loop (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.5, true, 2, true);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(1);
    input << 1.0;

    // Step 1
    Eigen::VectorXd out1 = net.feedForward(input);
    double h1_t1 = computeActivation(1.0 * 1.0 + 0.5 * 0.0, activationType);
    double expected1 = mapToOutput(computeActivation(h1_t1, activationType), activationType);
    assert(approxEqual(out1(0), expected1));
    std::cout << "  Step 1: " << out1(0) << " (expected: " << expected1 << ")\n";

    // Step 2
    Eigen::VectorXd out2 = net.feedForward(input);
    double h1_t2 = computeActivation(1.0 * 1.0 + 0.5 * h1_t1, activationType);
    double expected2 = mapToOutput(computeActivation(h1_t2, activationType), activationType);
    assert(approxEqual(out2(0), expected2));
    std::cout << "  Step 2: " << out2(0) << " (expected: " << expected2 << ")\n";

    // Step 3
    Eigen::VectorXd out3 = net.feedForward(input);
    double h1_t3 = computeActivation(1.0 * 1.0 + 0.5 * h1_t2, activationType);
    double expected3 = mapToOutput(computeActivation(h1_t3, activationType), activationType);
    assert(approxEqual(out3(0), expected3));
    std::cout << "  Step 3: " << out3(0) << " (expected: " << expected3 << ")\n";

    assert(notApproxEqual(out1(0), out2(0), 1e-9));
    std::cout << "  Memory effect verified (diff: " 
              << std::fabs(out1(0) - out2(0)) << ")\n";
    std::cout << "  ✓ Simple Recurrent Loop test passed\n";
}

void testRecurrentReset(ActivationType activationType) {
    std::cout << "\n🔄 Testing Recurrent Reset (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);
    genome.addNode(NodeType::HIDDEN);
    genome.addNode(NodeType::OUTPUT);

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.8, true, 2, true);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    net.feedForward(input);
    Eigen::VectorXd out3 = net.feedForward(input);
    
    std::cout << "  Before reset - Step 1: " << out1(0) << "\n";
    std::cout << "  Before reset - Step 3: " << out3(0) << "\n";

    net.reset();
    Eigen::VectorXd outAfterReset = net.feedForward(input);
    std::cout << "  After reset - Step 1: " << outAfterReset(0) << "\n";
    
    assert(approxEqual(out1(0), outAfterReset(0)));
    std::cout << "  ✓ Recurrent Reset test passed\n";
}

void testRecurrentWithChangingInput(ActivationType activationType) {
    std::cout << "\n🔄 Testing Recurrent with Changing Input (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);
    genome.addNode(NodeType::HIDDEN);
    genome.addNode(NodeType::OUTPUT);

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.5, true, 2, true);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input1(1), input2(1);
    input1 << 1.0;
    input2 << -1.0;

    Eigen::VectorXd out1 = net.feedForward(input1);
    std::cout << "  Step 1 (input=1.0): " << out1(0) << "\n";

    Eigen::VectorXd out2 = net.feedForward(input2);
    std::cout << "  Step 2 (input=-1.0, with memory): " << out2(0) << "\n";

    Eigen::VectorXd out3 = net.feedForward(input1);
    std::cout << "  Step 3 (input=1.0, with memory): " << out3(0) << "\n";

    assert(!approxEqual(out1(0), out3(0)));
    std::cout << "  ✓ Recurrent with Changing Input test passed\n";
}

void testRecurrentRocketScenario(ActivationType activationType) {
    std::cout << "\n🔄 Testing Rocket Control Scenario (" 
              << (activationType == TANH ? "TANH" : "SIGMOID") << ")...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // 0: target_x
    genome.addNode(NodeType::INPUT);   // 1: target_y
    genome.addNode(NodeType::INPUT);   // 2: vel_x
    genome.addNode(NodeType::INPUT);   // 3: vel_y
    genome.addNode(NodeType::HIDDEN);  // 4
    genome.addNode(NodeType::OUTPUT);  // 5: thrust
    genome.addNode(NodeType::OUTPUT);  // 6: rotation

    genome.addConnection(0, 4, 0.5, true, 0, false);
    genome.addConnection(1, 4, 0.5, true, 1, false);
    genome.addConnection(2, 4, 0.3, true, 2, false);
    genome.addConnection(3, 4, 0.3, true, 3, false);
    genome.addConnection(4, 5, 1.0, true, 4, false);
    genome.addConnection(4, 6, 1.0, true, 5, false);
    genome.addConnection(5, 4, 0.4, true, 6, true);
    genome.addConnection(6, 4, 0.4, true, 7, true);

    NeuralNetwork net(genome, activationType);

    Eigen::VectorXd input(4);
    input << 10.0, 5.0, -2.0, 1.0;
    
    Eigen::VectorXd out1 = net.feedForward(input);
    std::cout << "  Step 1 - Thrust: " << out1(0) << ", Rotation: " << out1(1) << "\n";
    
    for (int i = 2; i <= 4; ++i) {
        Eigen::VectorXd out = net.feedForward(input);
        std::cout << "  Step " << i << " - Thrust: " << out(0) 
                  << ", Rotation: " << out(1) << "\n";
    }
    
    net.reset();
    Eigen::VectorXd out_after_reset = net.feedForward(input);
    std::cout << "  After reset - Thrust: " << out_after_reset(0) 
              << ", Rotation: " << out_after_reset(1) << "\n";
    
    assert(approxEqual(out1(0), out_after_reset(0)));
    assert(approxEqual(out1(1), out_after_reset(1)));
    
    std::cout << "  ✓ Rocket Control Scenario test passed\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   RECURRENT TESTS (BOTH MODES)   \n";
    std::cout << "========================================\n";

    // Test with SIGMOID
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "         TESTING WITH SIGMOID          \n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    testSimpleRecurrentLoop(SIGMOID);
    testRecurrentReset(SIGMOID);
    testRecurrentWithChangingInput(SIGMOID);
    testRecurrentRocketScenario(SIGMOID);

    // Test with TANH
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "           TESTING WITH TANH           \n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    testSimpleRecurrentLoop(TANH);
    testRecurrentReset(TANH);
    testRecurrentWithChangingInput(TANH);
    testRecurrentRocketScenario(TANH);

    std::cout << "\n========================================\n";
    std::cout << "🎉 ALL RECURRENT TESTS PASSED (BOTH MODES)!\n";
    std::cout << "========================================\n";
    
    return 0;
}