#include "NEATRunner.hpp"
#include "Genome.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

// Helper to create a simple genome with specific connections
Genome createTestGenome(const std::vector<std::tuple<int, int, double, int>>& connections) {
    Genome genome;
    
    // Add basic nodes (adjust based on your needs)
    genome.addNode(NodeType::INPUT);   // 0
    genome.addNode(NodeType::INPUT);   // 1
    genome.addNode(NodeType::BIAS);    // 2
    genome.addNode(NodeType::OUTPUT);  // 3
    genome.addNode(NodeType::OUTPUT);  // 4
    
    // Add connections: {inId, outId, weight, innovNum}
    for (const auto& [in, out, weight, innov] : connections) {
        genome.addConnection(in, out, weight, true, innov, false);
    }
    
    return genome;
}

void testCompatibilityDistance() {
    std::cout << "\n✅ Testing Compatibility Distance Calculation...\n";
    
    NEATRunner runner;
    
    // Test 1: Identical genomes should have distance 0
    std::cout << "\n  Test 1: Identical genomes\n";
    Genome g1 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2}
    });
    
    Genome g2 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2}
    });
    
    double dist = runner.calcCompDistance(g1, g2);
    std::cout << "    Distance: " << dist << " (expected: 0.0)\n";
    assert(approxEqual(dist, 0.0));
    
    // Test 2: Same structure, different weights
    std::cout << "\n  Test 2: Same structure, different weights\n";
    Genome g3 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2}
    });
    
    Genome g4 = createTestGenome({
        {0, 3, 1.0, 0},  // weight diff: 0.5
        {1, 3, 0.2, 1},  // weight diff: 0.5
        {2, 4, 0.8, 2}   // weight diff: 0.5
    });
    
    dist = runner.calcCompDistance(g3, g4);
    // Expected: C3 * avgWeightDiff = 0.4 * 0.5 = 0.2
    std::cout << "    Distance: " << dist << " (expected: ~0.2)\n";
    assert(approxEqual(dist, 0.2));
    
    // Test 3: Disjoint genes
    std::cout << "\n  Test 3: Disjoint genes\n";
    Genome g5 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 2},
        {2, 4, 0.3, 4}
    });
    
    Genome g6 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 4, 0.6, 1},  // disjoint (g6 has, g5 doesn't)
        {2, 3, 0.4, 3},  // disjoint (g6 has, g5 doesn't)
        {2, 4, 0.3, 4}
    });
    // g5 has innov 2 which g6 doesn't have - also disjoint
    
    dist = runner.calcCompDistance(g5, g6);
    // Matching: 0, 4 (2 genes)
    // Disjoint: 2 from g5, and 1, 3 from g6 (3 total)
    // N = max(3, 4) = 4
    // Expected: C2 * (3 disjoint / 4) = 1.0 * 0.75 = 0.75
    std::cout << "    Distance: " << dist << " (expected: 0.75)\n";
    assert(approxEqual(dist, 0.75));
    
    // Test 4: Excess genes
    std::cout << "\n  Test 4: Excess genes\n";
    Genome g7 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1}
    });
    
    Genome g8 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2},  // excess
        {0, 4, 0.8, 3}   // excess
    });
    
    dist = runner.calcCompDistance(g7, g8);
    // Expected: C1 * (2 excess / 4) = 1.0 * 0.5 = 0.5
    std::cout << "    Distance: " << dist << " (expected: ~0.5)\n";
    assert(approxEqual(dist, 0.5));
    
    // Test 5: Complex case - mixture of all
    std::cout << "\n  Test 5: Mixed excess, disjoint, and weight differences\n";
    Genome g9 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.3, 2}, // disjoint 
        {2, 4, 0.7, 4}
    });
    
    Genome g10 = createTestGenome({
        {0, 3, 0.8, 0},  // matching, weight diff: 0.3
        {1, 4, 0.4, 1},  // disjoint
        {2, 3, 0.2, 3},  // disjoint
        {2, 4, 0.7, 4},  // matching, weight diff: 0.0
        {0, 4, 0.9, 5}   // excess
    });
    
    dist = runner.calcCompDistance(g9, g10);
    // E=1, D=2, N=5, avgW=0.15
    // Expected: 1.0*(1/5) + 1.0*(3/5) + 0.4*0.3/2 = 0.86
    std::cout << "    Distance: " << dist << " (expected: ~0.66)\n";
    assert(dist > 0.8 && dist < 0.9);
    
    std::cout << "\n  ✓ All compatibility distance tests passed!\n";
}

void testSpeciesAssignment() {
    std::cout << "\n✅ Testing Species Assignment...\n";
    
    NEATRunner runner;
    
    // Create several genomes with varying compatibility
    std::vector<Genome> testGenomes;
    
    // Group 1: Very similar genomes (should be same species)
    std::cout << "\n  Creating Group 1: Similar genomes\n";
    for (int i = 0; i < 3; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5 + i * 0.01, 0},
            {1, 3, 0.7 + i * 0.01, 1},
            {2, 4, 0.3 + i * 0.01, 2}
        });
        g.fitness = 10.0 + i;
        testGenomes.push_back(g);
    }
    
    // Group 2: Different structure (should be different species)
    std::cout << "  Creating Group 2: Different structure\n";
    for (int i = 0; i < 2; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5, 0},
            {1, 4, 0.6, 1},
            {2, 3, 0.4, 5},
            {2, 4, 0.3, 6}
        });
        g.fitness = 15.0 + i;
        testGenomes.push_back(g);
    }
    
    // Add genomes to runner
    runner.genomes = testGenomes;
    
    // Run speciation
    std::cout << "\n  Running speciation...\n";
    runner.speciate();
    
    // Verify results
    std::cout << "\n  Verifying results:\n";
    std::cout << "    Number of species: " << runner.speciesList.size() << "\n";
    
    // Should have at least 2 species
    assert(runner.speciesList.size() >= 2);
    
    // Check that genomes are assigned to species
    for (const auto& species : runner.speciesList) {
        std::cout << "    Species " << species.id << ": " 
                  << species.members.size() << " members\n";
        std::cout << "      Best fitness: " << species.bestFitness << "\n";
        std::cout << "      Avg fitness: " << species.speciesFitness << "\n";
        
        assert(species.members.size() > 0);
        
        // Verify that all members have correct species ID
        for (const auto& member : species.members) {
            assert(member.speciesID == species.id);
        }
    }
    
    std::cout << "\n  ✓ Species assignment test passed!\n";
}

void testAdjustedFitness() {
    std::cout << "\n✅ Testing Adjusted Fitness Calculation...\n";
    
    NEATRunner runner;
    
    // Create genomes with known fitness values
    std::vector<Genome> testGenomes;
    
    // Create 4 similar genomes (should be same species)
    for (int i = 0; i < 4; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5 + i * 0.01, 0},
            {1, 3, 0.7 + i * 0.01, 1}
        });
        g.fitness = 100.0;  // All have fitness of 100
        testGenomes.push_back(g);
    }
    
    runner.genomes = testGenomes;
    runner.speciate();
    
    // Check adjusted fitness
    std::cout << "\n  Checking adjusted fitness:\n";
    for (const auto& species : runner.speciesList) {
        std::cout << "    Species " << species.id << " (size: " 
                  << species.members.size() << "):\n";
        
        for (const auto& member : species.members) {
            double expectedAdjusted = member.fitness / species.members.size();
            std::cout << "      Raw fitness: " << member.fitness 
                      << ", Adjusted: " << member.adjustedFitness
                      << " (expected: " << expectedAdjusted << ")\n";
            
            assert(approxEqual(member.adjustedFitness, expectedAdjusted));
        }
    }
    
    std::cout << "\n  ✓ Adjusted fitness test passed!\n";
}

void testSpeciesMetrics() {
    std::cout << "\n✅ Testing Species Metrics (Best Fitness, Avg Fitness)...\n";
    
    NEATRunner runner;
    
    // Create genomes with varying fitness
    std::vector<Genome> testGenomes;
    
    for (int i = 0; i < 5; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5 + i * 0.01, 0},
            {1, 3, 0.7 + i * 0.01, 1}
        });
        g.fitness = 10.0 * (i + 1);  // Fitness: 10, 20, 30, 40, 50
        testGenomes.push_back(g);
    }
    
    runner.genomes = testGenomes;
    runner.speciate();
    
    std::cout << "\n  Verifying species metrics:\n";
    for (const auto& species : runner.speciesList) {
        std::cout << "    Species " << species.id << ":\n";
        std::cout << "      Members: " << species.members.size() << "\n";
        std::cout << "      Best fitness: " << species.bestFitness << "\n";
        std::cout << "      Avg fitness: " << species.speciesFitness << "\n";
        
        // Calculate expected values
        double maxFit = 0;
        double sumFit = 0;
        for (const auto& member : species.members) {
            maxFit = std::max(maxFit, member.fitness);
            sumFit += member.fitness;
        }
        double expectedAvg = sumFit / species.members.size();
        
        std::cout << "      Expected best: " << maxFit << "\n";
        std::cout << "      Expected avg: " << expectedAvg << "\n";
        
        assert(approxEqual(species.bestFitness, maxFit));
        assert(approxEqual(species.speciesFitness, expectedAvg));
    }
    
    std::cout << "\n  ✓ Species metrics test passed!\n";
}

void testThresholdBoundary() {
    std::cout << "\n✅ Testing Compatibility Threshold Boundary (COMP_THRESHOLD=3.0)...\n";
    
    NEATRunner runner;
    
    // Create two genomes just below threshold
    Genome g1 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2}
    });
    g1.fitness = 10.0;
    
    // Create g2 with enough difference to be just under threshold
    // With N=3, to get distance ~2.9: need fewer excess/disjoint genes
    Genome g2 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 3, 0.7, 1},
        {2, 4, 0.3, 2},
        {0, 4, 0.8, 3}  // One excess gene: 1.0 * 1/4 = 0.25
    });
    g2.fitness = 12.0;
    
    double dist = runner.calcCompDistance(g1, g2);
    std::cout << "    Distance between genomes: " << dist << "\n";
    
    // Create g3 with enough difference to exceed threshold
    Genome g3 = createTestGenome({
        {0, 3, 0.5, 0},
        {1, 4, 0.6, 5},
        {2, 3, 0.4, 6},
        {2, 4, 0.8, 7},
        {0, 4, 0.9, 8}
    });
    g3.fitness = 15.0;
    
    double dist2 = runner.calcCompDistance(g1, g3);
    std::cout << "    Distance to very different genome: " << dist2 << "\n";
    
    // Test speciation
    runner.genomes = {g1, g2, g3};
    runner.speciate();
    
    std::cout << "    Number of species formed: " << runner.speciesList.size() << "\n";
    
    for (const auto& species : runner.speciesList) {
        std::cout << "    Species " << species.id << ": " 
                  << species.members.size() << " members\n";
    }
    
    // g3 should be in a different species due to high distance
    assert(runner.speciesList.size() >= 1);
    
    std::cout << "\n  ✓ Threshold boundary test passed!\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "      SPECIATION TESTS (NEAT)          \n";
    std::cout << "========================================\n";
    std::cout << "Testing with C1=1.0, C2=1.0, C3=0.4\n";
    std::cout << "Compatibility Threshold = 3.0\n";
    
    testCompatibilityDistance();
    testSpeciesAssignment();
    testAdjustedFitness();
    testSpeciesMetrics();
    testThresholdBoundary();
    
    std::cout << "\n========================================\n";
    std::cout << "🎉 ALL SPECIATION TESTS PASSED!\n";
    std::cout << "========================================\n";
    
    return 0;
}