#include "NEATRunner.hpp"
#include "Genome.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <iomanip>

bool approxEqual(double a, double b, double eps = 1e-5)
{
    return std::fabs(a - b) < eps;
}

bool framesEqual(const ReplayFrame &f1, const ReplayFrame &f2, double eps = 1e-4)
{
    return f1.frame == f2.frame &&
           approxEqual(f1.rocketRotation, f2.rocketRotation, eps) &&
           approxEqual(f1.rocketThrust, f2.rocketThrust, eps) &&
           approxEqual(f1.rocketX, f2.rocketX, eps) &&
           approxEqual(f1.rocketY, f2.rocketY, eps) &&
           approxEqual(f1.targetX, f2.targetX, eps) &&
           approxEqual(f1.targetY, f2.targetY, eps) &&
           approxEqual(f1.vertex1.first, f2.vertex1.first, eps) &&
           approxEqual(f1.vertex1.second, f2.vertex1.second, eps) &&
           approxEqual(f1.vertex2.first, f2.vertex2.first, eps) &&
           approxEqual(f1.vertex2.second, f2.vertex2.second, eps) &&
           approxEqual(f1.vertex3.first, f2.vertex3.first, eps) &&
           approxEqual(f1.vertex3.second, f2.vertex3.second, eps);
}

// Helper to create a test genome
Genome createTestGenome(double fitness = 0.0)
{
    Genome genome;

    genome.addNode(NodeType::INPUT);  // 0 - target x
    genome.addNode(NodeType::INPUT);  // 1 - target y
    genome.addNode(NodeType::INPUT);  // 2 - vel x
    genome.addNode(NodeType::INPUT);  // 3 - vel y
    genome.addNode(NodeType::BIAS);   // 4
    genome.addNode(NodeType::OUTPUT); // 5 - thrust
    genome.addNode(NodeType::OUTPUT); // 6 - rotation

    // Add some connections
    genome.addConnection(0, 5, 0.5, true, 0, false);
    genome.addConnection(1, 5, 0.7, true, 1, false);
    genome.addConnection(2, 6, 0.3, true, 2, false);
    genome.addConnection(3, 6, 0.4, true, 3, false);
    genome.addConnection(4, 5, 0.2, true, 4, false);
    genome.addConnection(4, 6, 0.6, true, 5, false);

    genome.fitness = fitness;
    return genome;
}

void testEnvironmentResetDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 1: Environment Reset Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    std::cout << "\nChecking if Environment::reset() properly reinitializes state...\n";

    // Create first environment
    Environment env1(NEATRunner::ENV_WIDTH, NEATRunner::ENV_HEIGHT);
    double target1X = env1.target.pos(0);
    double target1Y = env1.target.pos(1);
    double rocket1X = env1.rocket.pos(0);
    double rocket1Y = env1.rocket.pos(1);

    std::cout << "First environment:\n";
    std::cout << "  Target: (" << target1X << ", " << target1Y << ")\n";
    std::cout << "  Rocket: (" << rocket1X << ", " << rocket1Y << ")\n";

    // Reset the environment
    env1.reset();
    double target1X_after = env1.target.pos(0);
    double target1Y_after = env1.target.pos(1);
    double rocket1X_after = env1.rocket.pos(0);
    double rocket1Y_after = env1.rocket.pos(1);

    std::cout << "After reset():\n";
    std::cout << "  Target: (" << target1X_after << ", " << target1Y_after << ")\n";
    std::cout << "  Rocket: (" << rocket1X_after << ", " << rocket1Y_after << ")\n";

    bool envResetDeterministic = (approxEqual(target1X, target1X_after) &&
                                  approxEqual(target1Y, target1Y_after) &&
                                  approxEqual(rocket1X, rocket1X_after) &&
                                  approxEqual(rocket1Y, rocket1Y_after));

    if (envResetDeterministic)
    {
        std::cout << "✅ Environment::reset() is DETERMINISTIC\n";
    }
    else
    {
        std::cout << "❌ Environment::reset() is NON-DETERMINISTIC\n";
        std::cout << "   Target changed: (" << target1X << "," << target1Y << ") -> ("
                  << target1X_after << "," << target1Y_after << ")\n";
        std::cout << "   Rocket changed: (" << rocket1X << "," << rocket1Y << ") -> ("
                  << rocket1X_after << "," << rocket1Y_after << ")\n";
    }

    return;
}

void testEnvironmentConstructorDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 2: Environment Constructor Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    std::cout << "\nCreating 3 separate environments and comparing initial states...\n";

    std::vector<double> targetXs, targetYs, rocketXs, rocketYs;

    for (int i = 0; i < 3; i++)
    {
        Environment env(NEATRunner::ENV_WIDTH, NEATRunner::ENV_HEIGHT);
        targetXs.push_back(env.target.pos(0));
        targetYs.push_back(env.target.pos(1));
        rocketXs.push_back(env.rocket.pos(0));
        rocketYs.push_back(env.rocket.pos(1));

        std::cout << "Environment " << i << ":\n";
        std::cout << "  Target: (" << env.target.pos(0) << ", " << env.target.pos(1) << ")\n";
        std::cout << "  Rocket: (" << env.rocket.pos(0) << ", " << env.rocket.pos(1) << ")\n";
    }

    // Check if all environments have same initial state
    bool allSame = true;
    for (int i = 1; i < 3; i++)
    {
        if (!approxEqual(targetXs[i], targetXs[0]) ||
            !approxEqual(targetYs[i], targetYs[0]) ||
            !approxEqual(rocketXs[i], rocketXs[0]) ||
            !approxEqual(rocketYs[i], rocketYs[0]))
        {
            allSame = false;
            break;
        }
    }

    if (allSame)
    {
        std::cout << "\n✅ Environment constructor initializes deterministically\n";
    }
    else
    {
        std::cout << "\n❌ Environment constructor is NON-DETERMINISTIC\n";
        std::cout << "   Different target positions spawned each time!\n";
    }
}

void testNeuralNetworkResetDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 3: Neural Network Reset Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    Genome genome = createTestGenome();

    std::cout << "\nCreating neural network and testing reset()...\n";
    std::cout << "Genome has " << genome.nodes.size() << " nodes and "
              << genome.connections.size() << " connections\n";

    NeuralNetwork net(genome, TANH);

    // Create an input vector
    Eigen::VectorXd input(4);
    input << 0.5, 0.3, 0.2, 0.1;

    // Forward pass
    Eigen::VectorXd output1 = net.feedForward(input);
    std::cout << "\nFirst feedForward output:\n";
    std::cout << "  [0]: " << output1(0) << ", [1]: " << output1(1) << "\n";

    // Reset
    net.reset();

    // Forward pass again with same input
    Eigen::VectorXd output2 = net.feedForward(input);
    std::cout << "\nAfter reset() + feedForward with same input:\n";
    std::cout << "  [0]: " << output2(0) << ", [1]: " << output2(1) << "\n";

    bool netResetDeterministic = (approxEqual(output1(0), output2(0)) &&
                                  approxEqual(output1(1), output2(1)));

    if (netResetDeterministic)
    {
        std::cout << "\n✅ Neural network reset() is DETERMINISTIC\n";
    }
    else
    {
        std::cout << "\n❌ Neural network reset() may have issues\n";
    }
}

void testSingleGenomeEvaluationDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 4: Single Genome Evaluation Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    NEATRunner runner;
    Genome genome = createTestGenome();

    std::cout << "\nEvaluating the same genome 3 times and comparing results...\n";

    std::vector<double> fitnesses;
    std::vector<std::vector<ReplayFrame>> frameRuns;

    for (int run = 0; run < 3; run++)
    {
        Environment env(NEATRunner::ENV_WIDTH, NEATRunner::ENV_HEIGHT);
        NeuralNetwork net(genome, TANH);

        double fitness = runner.evaluateGenome(genome, net, env);
        fitnesses.push_back(fitness);

        std::cout << "Run " << run << ": fitness = " << std::fixed << std::setprecision(6)
                  << fitness << "\n";
    }

    // Check if all fitnesses match
    bool fitnessesDeterministic = true;
    for (int i = 1; i < 3; i++)
    {
        if (!approxEqual(fitnesses[i], fitnesses[0], 1e-4))
        {
            fitnessesDeterministic = false;
            std::cout << "  ❌ Fitness mismatch: Run 0 = " << fitnesses[0]
                      << ", Run " << i << " = " << fitnesses[i] << "\n";
        }
    }

    if (fitnessesDeterministic)
    {
        std::cout << "\n✅ Single genome evaluation is DETERMINISTIC (fitness consistent)\n";
    }
    else
    {
        std::cout << "\n❌ Single genome evaluation is NON-DETERMINISTIC\n";
    }
}

void testReplayFramesDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 5: Replay Frames Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    NEATRunner runner;
    Genome genome = createTestGenome();

    std::cout << "\nEvaluating genome with replay=true and comparing frame sequences...\n";
    std::cout << "Note: This is more strict than fitness - requires exact frame matching\n";

    std::vector<std::vector<ReplayFrame>> runs;

    for (int run = 0; run < 3; run++)
    {
        Environment env(NEATRunner::ENV_WIDTH, NEATRunner::ENV_HEIGHT);
        NeuralNetwork net(genome, TANH);

        std::vector<ReplayFrame> frames = runner.evaluateGenome(genome, net, env, true);
        runs.push_back(frames);

        std::cout << "Run " << run << ": captured " << frames.size() << " frames\n";
    }

    // Compare frame counts
    bool frameCountsDeterministic = true;
    for (int i = 1; i < 3; i++)
    {
        if (runs[i].size() != runs[0].size())
        {
            frameCountsDeterministic = false;
            std::cout << "  ❌ Frame count mismatch: Run 0 = " << runs[0].size()
                      << ", Run " << i << " = " << runs[i].size() << "\n";
        }
    }

    if (!frameCountsDeterministic)
    {
        std::cout << "\n❌ Replay frames frame count is NON-DETERMINISTIC\n";
        return;
    }

    // Compare individual frames
    bool framesDeterministic = true;
    if (runs[0].size() > 0)
    {
        // Sample first, middle, and last frames
        std::vector<int> sampleIndices = {0, (int)runs[0].size() / 2, (int)runs[0].size() - 1};

        for (int idx : sampleIndices)
        {
            if (idx >= runs[0].size())
                continue;

            std::cout << "\n  Comparing frame " << idx << ":\n";

            for (int run = 1; run < 3; run++)
            {
                if (!framesEqual(runs[0][idx], runs[run][idx]))
                {
                    framesDeterministic = false;
                    std::cout << "    ❌ Frame " << idx << " differs between Run 0 and Run " << run << "\n";
                    std::cout << "       Run 0: pos=(" << runs[0][idx].rocketX << ","
                              << runs[0][idx].rocketY << ")\n";
                    std::cout << "       Run " << run << ": pos=(" << runs[run][idx].rocketX << ","
                              << runs[run][idx].rocketY << ")\n";
                }
                else
                {
                    std::cout << "    ✅ Frame " << idx << " matches between Run 0 and Run " << run << "\n";
                }
            }
        }
    }

    if (framesDeterministic && frameCountsDeterministic)
    {
        std::cout << "\n✅ Replay frames are DETERMINISTIC (exact match)\n";
    }
    else
    {
        std::cout << "\n❌ Replay frames are NON-DETERMINISTIC\n";
    }
}

void testPopulationEvaluationConsistency()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 6: Population Evaluation Consistency\n";
    std::cout << std::string(70, '=') << "\n";

    NEATRunner runner1;
    NEATRunner runner2;

    std::cout << "\nCreating two identical NEATRunner instances...\n";
    std::cout << "Population size: " << NEATRunner::POP_SIZE << "\n";

    // Both should start with identical genomes (since RNG is seeded randomly)
    // So we'll just test if evaluation is consistent within one runner

    std::cout << "\nEvaluating entire population once, then re-evaluating...\n";

    runner1.testOutGenomes();

    std::vector<double> firstRun;
    for (const auto &genome : runner1.genomes)
    {
        firstRun.push_back(genome.fitness);
    }

    // Re-evaluate
    runner1.testOutGenomes();

    std::vector<double> secondRun;
    for (const auto &genome : runner1.genomes)
    {
        secondRun.push_back(genome.fitness);
    }

    // Compare
    bool populationConsistent = true;
    int mismatches = 0;
    for (size_t i = 0; i < firstRun.size(); i++)
    {
        if (!approxEqual(firstRun[i], secondRun[i], 1e-4))
        {
            populationConsistent = false;
            mismatches++;
        }
    }

    std::cout << "\nComparison of two population evaluations:\n";
    std::cout << "  Total genomes: " << firstRun.size() << "\n";
    std::cout << "  Fitness mismatches: " << mismatches << "\n";

    if (mismatches > 0 && mismatches <= 3)
    {
        std::cout << "  Sample mismatches:\n";
        int shown = 0;
        for (size_t i = 0; i < firstRun.size() && shown < 3; i++)
        {
            if (!approxEqual(firstRun[i], secondRun[i], 1e-4))
            {
                std::cout << "    Genome " << i << ": " << firstRun[i] << " vs "
                          << secondRun[i] << "\n";
                shown++;
            }
        }
    }

    if (populationConsistent)
    {
        std::cout << "\n✅ Population evaluation is CONSISTENT\n";
    }
    else
    {
        std::cout << "\n❌ Population evaluation is INCONSISTENT\n";
    }
}

void testBestGenomeDeterminism()
{
    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "TEST 7: Best Genome Replay Determinism\n";
    std::cout << std::string(70, '=') << "\n";

    std::cout << "\nRunning a full generation and replaying the best genome...\n";

    NEATRunner runner;

    // Run one generation
    std::cout << "Running generation...\n";
    runner.runGeneration();

    Genome bestGenome = runner.bestPerformingGenome;
    std::cout << "Best genome fitness: " << bestGenome.fitness << "\n";
    std::cout << "Best genome species: " << bestGenome.speciesID << "\n";

    // Replay multiple times
    std::cout << "\nReplaying best genome 3 times...\n";
    std::vector<double> replayFitnesses;
    std::vector<size_t> frameCountsReplay;

    for (int replay = 0; replay < 3; replay++)
    {
        Environment replayEnv(NEATRunner::ENV_WIDTH, NEATRunner::ENV_HEIGHT);
        NeuralNetwork replayNet(bestGenome, TANH);

        std::vector<ReplayFrame> frames = runner.evaluateGenome(bestGenome, replayNet, replayEnv, true);

        frameCountsReplay.push_back(frames.size());
        std::cout << "Replay " << replay << ": " << frames.size() << " frames\n";

        // Calculate fitness from frames
        double fitness = runner.evaluateGenome(bestGenome, replayNet, replayEnv);
        replayFitnesses.push_back(fitness);
        std::cout << "  Fitness: " << fitness << "\n";
    }

    // Check consistency
    bool replayDeterministic = true;
    for (int i = 1; i < 3; i++)
    {
        if (frameCountsReplay[i] != frameCountsReplay[0])
        {
            replayDeterministic = false;
            std::cout << "  ❌ Frame count mismatch\n";
        }
        if (!approxEqual(replayFitnesses[i], replayFitnesses[0], 1e-3))
        {
            replayDeterministic = false;
            std::cout << "  ❌ Fitness mismatch\n";
        }
    }

    if (replayDeterministic)
    {
        std::cout << "\n✅ Best genome replay is DETERMINISTIC\n";
    }
    else
    {
        std::cout << "\n❌ Best genome replay is NON-DETERMINISTIC\n";
    }
}

int main()
{
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         COMPREHENSIVE DETERMINISM TEST SUITE FOR NEAT             ║\n";
    std::cout << "║                   (Debugging Replay Issues)                        ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════════╝\n";

    std::cout << "\n⚠️  IMPORTANT: This test suite will identify where non-determinism occurs.\n";
    std::cout << "If any test fails, it indicates a source of randomness in your system.\n\n";

    testEnvironmentResetDeterminism();
    testEnvironmentConstructorDeterminism();
    testNeuralNetworkResetDeterminism();
    testSingleGenomeEvaluationDeterminism();
    testReplayFramesDeterminism();
    testPopulationEvaluationConsistency();
    testBestGenomeDeterminism();

    std::cout << "\n"
              << std::string(70, '=') << "\n";
    std::cout << "🎉 DETERMINISM TEST SUITE COMPLETE\n";
    std::cout << std::string(70, '=') << "\n\n";

    std::cout << "SUMMARY CHECKLIST:\n";
    std::cout << "[ ] Test 1: Environment Reset Determinism\n";
    std::cout << "[ ] Test 2: Environment Constructor Determinism\n";
    std::cout << "[ ] Test 3: Neural Network Reset Determinism\n";
    std::cout << "[ ] Test 4: Single Genome Evaluation Determinism\n";
    std::cout << "[ ] Test 5: Replay Frames Determinism\n";
    std::cout << "[ ] Test 6: Population Evaluation Consistency\n";
    std::cout << "[ ] Test 7: Best Genome Replay Determinism\n\n";

    std::cout << "Review the output above and identify which tests fail.\n";
    std::cout << "The failing tests will guide you to the source of randomness.\n\n";

    return 0;
}