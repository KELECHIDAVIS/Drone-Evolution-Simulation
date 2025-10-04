
#include "Genome.hpp"
#include "NeuralNetwork.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <cassert>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

auto sigmoid = [](double x) { return 1.0 / (1.0 + std::exp(-4.9 * x)); };

// ==================== BASIC RECURRENT TESTS ====================

void testSimpleRecurrentLoop() {
    std::cout << "\n🔄 Testing Simple Recurrent Loop...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);  // input -> hidden
    genome.addConnection(1, 2, 1.0, true, 1, false);  // hidden -> output
    genome.addConnection(1, 1, 0.5, true, 2, true);   // hidden -> hidden (RECURRENT)

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    // First timestep: recurrent connection uses 0.0 (initial state)
    Eigen::VectorXd out1 = net.feedForward(input);
    double h1_t1 = sigmoid(1.0 * 1.0 + 0.5 * 0.0);
    double expected1 = sigmoid(h1_t1);
    assert(approxEqual(out1(0), expected1));
    std::cout << "  Step 1: " << out1(0) << " (expected: " << expected1 << ")\n";

    // Second timestep: recurrent connection uses h1_t1
    Eigen::VectorXd out2 = net.feedForward(input);
    double h1_t2 = sigmoid(1.0 * 1.0 + 0.5 * h1_t1);
    double expected2 = sigmoid(h1_t2);
    assert(approxEqual(out2(0), expected2));
    std::cout << "  Step 2: " << out2(0) << " (expected: " << expected2 << ")\n";

    // Third timestep: recurrent connection uses h1_t2
    Eigen::VectorXd out3 = net.feedForward(input);
    double h1_t3 = sigmoid(1.0 * 1.0 + 0.5 * h1_t2);
    double expected3 = sigmoid(h1_t3);
    assert(approxEqual(out3(0), expected3));
    std::cout << "  Step 3: " << out3(0) << " (expected: " << expected3 << ")\n";

    // Verify outputs are different (showing memory effect)
    assert(!approxEqual(out1(0), out2(0)));
    assert(!approxEqual(out2(0), out3(0)));

    std::cout << "✅ Simple Recurrent Loop test passed\n";
}

void testRecurrentReset() {
    std::cout << "\n🔄 Testing Recurrent Reset...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.8, true, 2, true);

    NeuralNetwork net(genome);

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

    std::cout << "✅ Recurrent Reset test passed\n";
}

void testRecurrentWithChangingInput() {
    std::cout << "\n🔄 Testing Recurrent with Changing Input...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.5, true, 2, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input1(1), input2(1);
    input1 << 1.0;
    input2 << -1.0;

    Eigen::VectorXd out1 = net.feedForward(input1);
    double h1_t1 = sigmoid(1.0 * 1.0);
    std::cout << "  Step 1 (input=1.0): " << out1(0) << "\n";

    Eigen::VectorXd out2 = net.feedForward(input2);
    double h1_t2 = sigmoid(1.0 * (-1.0) + 0.5 * h1_t1);
    double expected2 = sigmoid(h1_t2);
    assert(approxEqual(out2(0), expected2));
    std::cout << "  Step 2 (input=-1.0, memory from step 1): " << out2(0) << "\n";

    Eigen::VectorXd out3 = net.feedForward(input1);
    std::cout << "  Step 3 (input=1.0, memory from step 2): " << out3(0) << "\n";

    assert(!approxEqual(out1(0), out3(0)));

    std::cout << "✅ Recurrent with Changing Input test passed\n";
}

void testRecurrentBetweenLayers() {
    std::cout << "\n🔄 Testing Recurrent Between Layers...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(2, 3, 1.0, true, 2, false);
    genome.addConnection(2, 1, 0.5, true, 3, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    double h1_t1 = sigmoid(1.0);
    double h2_t1 = sigmoid(h1_t1);
    double expected1 = sigmoid(h2_t1);
    std::cout << "  Step 1: " << out1(0) << " (expected: " << expected1 << ")\n";
    assert(approxEqual(out1(0), expected1));

    Eigen::VectorXd out2 = net.feedForward(input);
    double h1_t2 = sigmoid(1.0 + 0.5 * h2_t1);
    double h2_t2 = sigmoid(h1_t2);
    double expected2 = sigmoid(h2_t2);
    std::cout << "  Step 2: " << out2(0) << " (expected: " << expected2 << ")\n";
    assert(approxEqual(out2(0), expected2));

    assert(!approxEqual(out1(0), out2(0)));

    std::cout << "✅ Recurrent Between Layers test passed\n";
}

void testMultipleRecurrentConnections() {
    std::cout << "\n🔄 Testing Multiple Recurrent Connections...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(0, 2, 1.0, true, 1, false);
    genome.addConnection(1, 3, 0.5, true, 2, false);
    genome.addConnection(2, 3, 0.5, true, 3, false);
    genome.addConnection(1, 1, 0.3, true, 4, true);
    genome.addConnection(2, 2, 0.3, true, 5, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    double h1_t1 = sigmoid(1.0);
    double h2_t1 = sigmoid(1.0);
    double expected1 = sigmoid(0.5 * h1_t1 + 0.5 * h2_t1);
    std::cout << "  Step 1: " << out1(0) << " (expected: " << expected1 << ")\n";
    assert(approxEqual(out1(0), expected1));

    Eigen::VectorXd out2 = net.feedForward(input);
    double h1_t2 = sigmoid(1.0 + 0.3 * h1_t1);
    double h2_t2 = sigmoid(1.0 + 0.3 * h2_t1);
    double expected2 = sigmoid(0.5 * h1_t2 + 0.5 * h2_t2);
    std::cout << "  Step 2: " << out2(0) << " (expected: " << expected2 << ")\n";
    assert(approxEqual(out2(0), expected2));

    std::cout << "✅ Multiple Recurrent Connections test passed\n";
}

void testRecurrentToOutput() {
    std::cout << "\n🔄 Testing Recurrent to Output...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::OUTPUT);  // id = 1

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 1, 0.5, true, 1, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    double expected1 = sigmoid(1.0);
    std::cout << "  Step 1: " << out1(0) << " (expected: " << expected1 << ")\n";
    assert(approxEqual(out1(0), expected1));

    Eigen::VectorXd out2 = net.feedForward(input);
    double expected2 = sigmoid(1.0 + 0.5 * expected1);
    std::cout << "  Step 2: " << out2(0) << " (expected: " << expected2 << ")\n";
    assert(approxEqual(out2(0), expected2));

    std::cout << "✅ Recurrent to Output test passed\n";
}

void testMixedFeedforwardAndRecurrent() {
    std::cout << "\n🔄 Testing Mixed Feedforward and Recurrent...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::HIDDEN);  // id = 2
    genome.addNode(NodeType::OUTPUT);  // id = 3

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(2, 2, 0.5, true, 2, true);
    genome.addConnection(2, 3, 1.0, true, 3, false);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    double h1_t1 = sigmoid(1.0);
    double h2_t1 = sigmoid(h1_t1 + 0.5 * 0.0);
    double expected1 = sigmoid(h2_t1);
    std::cout << "  Step 1: " << out1(0) << "\n";
    assert(approxEqual(out1(0), expected1));

    Eigen::VectorXd out2 = net.feedForward(input);
    double h1_t2 = sigmoid(1.0);
    double h2_t2 = sigmoid(h1_t2 + 0.5 * h2_t1);
    double expected2 = sigmoid(h2_t2);
    std::cout << "  Step 2: " << out2(0) << "\n";
    assert(approxEqual(out2(0), expected2));

    std::cout << "✅ Mixed Feedforward and Recurrent test passed\n";
}

void testDisabledRecurrentConnection() {
    std::cout << "\n🔄 Testing Disabled Recurrent Connection...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.5, false, 2, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    Eigen::VectorXd out2 = net.feedForward(input);

    assert(approxEqual(out1(0), out2(0)));
    std::cout << "  Step 1: " << out1(0) << "\n";
    std::cout << "  Step 2: " << out2(0) << " (should be same)\n";

    std::cout << "✅ Disabled Recurrent Connection test passed\n";
}

void testZeroWeightRecurrent() {
    std::cout << "\n🔄 Testing Zero Weight Recurrent...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.0, true, 2, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    Eigen::VectorXd out2 = net.feedForward(input);

    assert(approxEqual(out1(0), out2(0)));
    std::cout << "  Step 1: " << out1(0) << "\n";
    std::cout << "  Step 2: " << out2(0) << " (should be same)\n";

    std::cout << "✅ Zero Weight Recurrent test passed\n";
}

void testRecurrentMemoryDecay() {
    std::cout << "\n🔄 Testing Recurrent Memory Decay...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0
    genome.addNode(NodeType::HIDDEN);  // id = 1
    genome.addNode(NodeType::OUTPUT);  // id = 2

    genome.addConnection(0, 1, 1.0, true, 0, false);
    genome.addConnection(1, 2, 1.0, true, 1, false);
    genome.addConnection(1, 1, 0.2, true, 2, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(1);
    input << 1.0;

    Eigen::VectorXd out1 = net.feedForward(input);
    std::cout << "  Initial: " << out1(0) << "\n";

    input << 0.0;
    std::vector<double> outputs;
    for (int i = 0; i < 5; i++) {
        Eigen::VectorXd out = net.feedForward(input);
        outputs.push_back(out(0));
        std::cout << "  Step " << (i+2) << " (no input): " << out(0) << "\n";
    }

    double sigmoidZero = sigmoid(0.0);
    for (size_t i = 1; i < outputs.size(); i++) {
        assert(std::fabs(outputs[i] - sigmoidZero) < std::fabs(outputs[i-1] - sigmoidZero));
    }

    std::cout << "✅ Recurrent Memory Decay test passed\n";
}

void testRecurrentRocketScenario() {
    std::cout << "\n🔄 Testing Rocket Control Scenario...\n";
    
    Genome genome;
    genome.addNode(NodeType::INPUT);   // id = 0 (target_x)
    genome.addNode(NodeType::INPUT);   // id = 1 (target_y)
    genome.addNode(NodeType::INPUT);   // id = 2 (vel_x)
    genome.addNode(NodeType::INPUT);   // id = 3 (vel_y)
    genome.addNode(NodeType::HIDDEN);  // id = 4
    genome.addNode(NodeType::OUTPUT);  // id = 5 (thrust)
    genome.addNode(NodeType::OUTPUT);  // id = 6 (rotation)

    genome.addConnection(0, 4, 0.5, true, 0, false);
    genome.addConnection(1, 4, 0.5, true, 1, false);
    genome.addConnection(2, 4, 0.3, true, 2, false);
    genome.addConnection(3, 4, 0.3, true, 3, false);
    genome.addConnection(4, 5, 1.0, true, 4, false);
    genome.addConnection(4, 6, 1.0, true, 5, false);
    genome.addConnection(5, 4, 0.4, true, 6, true);
    genome.addConnection(6, 4, 0.4, true, 7, true);

    NeuralNetwork net(genome);

    Eigen::VectorXd input(4);
    input << 10.0, 5.0, -2.0, 1.0;
    
    Eigen::VectorXd out1 = net.feedForward(input);
    std::cout << "  Step 1 - Thrust: " << out1(0) << ", Rotation: " << out1(1) << "\n";
    
    Eigen::VectorXd out2 = net.feedForward(input);
    std::cout << "  Step 2 - Thrust: " << out2(0) << ", Rotation: " << out2(1) << "\n";
    
    assert(!approxEqual(out1(0), out2(0)) || !approxEqual(out1(1), out2(1)));
    
    net.reset();
    Eigen::VectorXd out3 = net.feedForward(input);
    std::cout << "  After reset - Thrust: " << out3(0) << ", Rotation: " << out3(1) << "\n";
    
    assert(approxEqual(out1(0), out3(0)));
    assert(approxEqual(out1(1), out3(1)));

    std::cout << "✅ Rocket Control Scenario test passed\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "   RECURRENT NEAT FUNCTIONALITY TESTS   \n";
    std::cout << "========================================\n";

    testSimpleRecurrentLoop();
    testRecurrentReset();
    testRecurrentWithChangingInput();
    testRecurrentBetweenLayers();
    testMultipleRecurrentConnections();
    testRecurrentToOutput();
    testMixedFeedforwardAndRecurrent();
    testDisabledRecurrentConnection();
    testZeroWeightRecurrent();
    testRecurrentMemoryDecay();
    testRecurrentRocketScenario();

    std::cout << "\n========================================\n";
    std::cout << "🎉 ALL RECURRENT TESTS PASSED!\n";
    std::cout << "========================================\n";
    
    return 0;
}
