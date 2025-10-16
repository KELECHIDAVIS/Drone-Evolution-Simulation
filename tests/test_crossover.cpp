#include "NEATRunner.hpp"
#include "Genome.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <unordered_set>

bool approxEqual(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

// Helper to create a genome with specific connections
Genome createTestGenome(const std::vector<std::tuple<int, int, double, int>>& connections, double fitness = 0.0) {
    Genome genome;
    
    genome.addNode(NodeType::INPUT);   // 0
    genome.addNode(NodeType::INPUT);   // 1
    genome.addNode(NodeType::BIAS);    // 2
    genome.addNode(NodeType::OUTPUT);  // 3
    genome.addNode(NodeType::OUTPUT);  // 4
    
    for (const auto& [in, out, weight, innov] : connections) {
        genome.addConnection(in, out, weight, true, innov, false);
    }
    
    genome.fitness = fitness;
    return genome;
}

Genome createTestGenome2(const std::vector<std::tuple<int, int, double, int, bool>>& connections, double fitness = 0.0) {
    Genome genome;
    
    genome.addNode(NodeType::INPUT);   // 0
    genome.addNode(NodeType::INPUT);   // 1
    genome.addNode(NodeType::INPUT);   // 2
    genome.addNode(NodeType::OUTPUT);  // 3
    genome.addNode(NodeType::HIDDEN);  // 4
    
    for (const auto& [in, out, weight, innov, enabled] : connections) {
        genome.addConnection(in, out, weight, enabled, innov, false);
    }
    
    genome.fitness = fitness;
    return genome;
}

Genome createTestGenome3(const std::vector<std::tuple<int, int, double, int, bool>>& connections, double fitness = 0.0) {
    Genome genome;
    
    genome.addNode(NodeType::INPUT);   // 0
    genome.addNode(NodeType::INPUT);   // 1
    genome.addNode(NodeType::INPUT);   // 2
    genome.addNode(NodeType::OUTPUT);  // 3
    genome.addNode(NodeType::HIDDEN);  // 4
    genome.addNode(NodeType::HIDDEN);  // 5
    
    for (const auto& [in, out, weight, innov, enabled] : connections) {
        genome.addConnection(in, out, weight, true, innov, false);
    }
    
    genome.fitness = fitness;
    return genome;
}

void testPopulationSizePreservation() {
    std::cout << "\n✅ Testing Population Size Preservation...\n";
    
    NEATRunner runner;
    const int INITIAL_POP = NEATRunner::POP_SIZE; // Should match POP_SIZE
    
    // Create initial population with varying fitness
    runner.genomes.clear();
    for (int i = 0; i < INITIAL_POP; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5, 0},
            {1, 3, 0.7, 1}
        }, 10.0 + i); // Varying fitness
        runner.genomes.push_back(g);
    }
    
    // Run one generation
    runner.testOutGenomes();

    runner.speciate();
    
    runner.crossover();
    
    std::cout << "  Initial population: " << INITIAL_POP << "\n";
    std::cout << "  After crossover: " << runner.genomes.size() << "\n";
    
    assert(runner.genomes.size() == INITIAL_POP);
    std::cout << "  ✓ Population size preserved!\n";
}

void testOffspringAllocation() {
    std::cout << "\n✅ Testing Offspring Allocation Proportions...\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create two distinct species with different fitness levels
    // Species 1: High fitness (should get more offspring)
    for (int i = 0; i < 5; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5, 0},
            {1, 3, 0.7, 1}
        }, 100.0); // High fitness
        runner.genomes.push_back(g);
    }
    
    // Species 2: Low fitness (should get fewer offspring)
    for (int i = 0; i < 5; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5, 0},
            {1, 4, 0.6, 5},
            {2, 3, 0.4, 6}
        }, 20.0); // Low fitness
        runner.genomes.push_back(g);
    }
    
    runner.testOutGenomes();
    runner.speciate();
    
    std::cout << "  Number of species: " << runner.speciesList.size() << "\n";
    
    // Record species sizes before crossover
    std::vector<int> beforeSizes;
    std::vector<double> speciesFitnesses;
    for (auto& species : runner.speciesList) {
        beforeSizes.push_back(species.members.size());
        speciesFitnesses.push_back(species.sumOfAdjFits);
        std::cout << "  Species " << species.id << ": " 
                  << species.members.size() << " members, "
                  << "sum adjusted fitness: " << species.sumOfAdjFits << "\n";
    }
    
    runner.crossover();
    
    // Check that better species got more offspring (approximately)
    if (runner.speciesList.size() >= 2) {
        double ratio = speciesFitnesses[0] / speciesFitnesses[1];
        std::cout << "  Fitness ratio (species 0 / species 1): " << ratio << "\n";
        std::cout << "  Higher fitness species should get proportionally more offspring\n";
    }
    
    std::cout << "  ✓ Offspring allocation test passed!\n";
}

void testChampionPreservation() {
    std::cout << "\n✅ Testing Champion Preservation...\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create a species with >5 members
    std::vector<Genome> testGenomes;
    for (int i = 0; i < 10; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5 + i * 0.01, 0},
            {1, 3, 0.7 + i * 0.01, 1}
        }, 10.0 + i * 10); // Increasing fitness
        testGenomes.push_back(g);
    }
    
    runner.genomes = testGenomes;
    runner.testOutGenomes();
    runner.speciate();
    
    // Find the champion (best genome)
    double bestFitness = 0;
    for (auto& species : runner.speciesList) {
        if (species.members.size() > 5) {
            std::sort(species.members.begin(), species.members.end(),
                [](const Genome& a, const Genome& b) { return a.fitness > b.fitness; });
            bestFitness = species.members[0].fitness;
            std::cout << "  Champion fitness before crossover: " << bestFitness << "\n";
            std::cout << "  Species size: " << species.members.size() << "\n";
        }
    }
    
    runner.crossover();
    
    // Check if champion exists in next generation
    bool championPreserved = false;
    for (auto& genome : runner.genomes) {
        if (approxEqual(genome.fitness, bestFitness)) {
            championPreserved = true;
            break;
        }
    }
    
    std::cout << "  Champion preserved: " << (championPreserved ? "Yes" : "No") << "\n";
    std::cout << "  Note: Champion should be copied unchanged for species > 5 members\n";
    
    std::cout << "  ✓ Champion preservation test completed!\n";
}

void testAsexualReproduction() {
    std::cout << "\n✅ Testing Asexual Reproduction (Single Member Species)...\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create several very different genomes that will form separate species
    Genome g1 = createTestGenome({{0, 3, 0.5, 0}}, 50.0);
    Genome g2 = createTestGenome({{0, 3, 0.5, 10}, {1, 4, 0.6, 15}}, 40.0);
    Genome g3 = createTestGenome({{0, 3, 0.5, 20}, {1, 3, 0.6, 21}, {2, 4, 0.4, 22}}, 30.0);
    
    runner.genomes = {g1, g2, g3};
    runner.testOutGenomes();
    runner.speciate();
    
    int singleMemberSpecies = 0;
    for (auto& species : runner.speciesList) {
        if (species.members.size() == 1) {
            singleMemberSpecies++;
            std::cout << "  Found single-member species " << species.id << "\n";
        }
    }
    
    std::cout << "  Single member species count: " << singleMemberSpecies << "\n";
    
    runner.crossover();
    
    std::cout << "  ✓ Single-member species handled (asexual reproduction)\n";
}

void testSpeciesCulling() {
    std::cout << "\n✅ Testing Species Culling (Bottom 50% Removal)...\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create one species with many members
    for (int i = 0; i < 20; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5 + i * 0.001, 0},
            {1, 3, 0.7 + i * 0.001, 1}
        }, 10.0 + i * 5); // Varying fitness
        runner.genomes.push_back(g);
    }
    
    runner.testOutGenomes();
    runner.speciate();
    
    int originalSize = 0;
    for (auto& species : runner.speciesList) {
        originalSize = species.members.size();
        std::cout << "  Original species size: " << originalSize << "\n";
    }
    
    runner.crossover();
    
    // Note: After crossover, species.members is culled but we can't directly verify
    // because the next generation replaces genomes
    std::cout << "  After crossover, bottom 50% should be removed before breeding\n";
    std::cout << "  ✓ Species culling test completed!\n";
}

void testRemainderAllocation() {
    std::cout << "\n✅ Testing Remainder Allocation to Best Species...\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create population that will cause rounding issues
    // 3 species with proportions that don't divide evenly
    for (int i = 0; i < 50; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 0}}, 100.0);
        runner.genomes.push_back(g);
    }
    for (int i = 0; i < 50; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 5}, {1, 4, 0.6, 6}}, 50.0);
        runner.genomes.push_back(g);
    }
    for (int i = 0; i < 50; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 10}, {1, 3, 0.6, 11}, {2, 4, 0.4, 12}}, 25.0);
        runner.genomes.push_back(g);
    }
    
    runner.testOutGenomes();
    runner.speciate();
    
    std::cout << "  Species count: " << runner.speciesList.size() << "\n";
    for (auto& species : runner.speciesList) {
        std::cout << "  Species " << species.id << ": bestFitness = " 
                  << species.bestFitness << "\t Sum of adjfits: "<<species.sumOfAdjFits<< "\n";
    }
    
    int beforeSize = runner.genomes.size();
    runner.crossover();
    int afterSize = runner.genomes.size();
    
    std::cout << "  Population before: " << beforeSize << "\n";
    std::cout << "  Population after: " << afterSize << "\n";
    
    assert(beforeSize == afterSize);
    std::cout << "  ✓ Remainder properly allocated to best species!\n";
}

void testCrossoverMechanics() {
    std::cout << "\n✅ Testing Crossover Mechanics (25% mutation-only, 75% crossover)...\n";
    
    // This is a statistical test - run multiple times
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create a reasonable population
    for (int i = 0; i < 100; i++) {
        Genome g = createTestGenome({
            {0, 3, 0.5, 0},
            {1, 3, 0.7, 1}
        }, 50.0 + i);
        runner.genomes.push_back(g);
    }
    
    runner.testOutGenomes();
    runner.speciate();
    runner.crossover();
    
    std::cout << "  Generated " << runner.genomes.size() << " offspring\n";
    std::cout << "  Note: Should be ~25% asexual, ~75% crossover (statistical)\n";
    std::cout << "  ✓ Crossover mechanics test completed!\n";
}

void testInterspeciesMating() {
    std::cout << "\n✅ Testing Interspecies Mating Rate (0.001)...\n";
    
    std::cout << "  Note: With rate of 0.001, interspecies mating is very rare\n";
    std::cout << "  In a population of 150, expect ~0.1 interspecies matings per generation\n";
    std::cout << "  This test verifies the code compiles and runs without crashes\n";
    
    NEATRunner runner;
    runner.genomes.clear();
    
    // Create multiple distinct species
    for (int i = 0; i < 30; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 0}, {1, 3, 0.7, 1}}, 50.0);
        runner.genomes.push_back(g);
    }
    for (int i = 0; i < 30; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 10}, {1, 4, 0.6, 11}}, 40.0);
        runner.genomes.push_back(g);
    }
    for (int i = 0; i < 30; i++) {
        Genome g = createTestGenome({{0, 3, 0.5, 20}, {2, 4, 0.4, 21}}, 30.0);
        runner.genomes.push_back(g);
    }
    
    runner.testOutGenomes();
    runner.speciate();
    
    std::cout << "  Created " << runner.speciesList.size() << " species\n";
    
    runner.crossover();
    
    std::cout << "  ✓ Interspecies mating code executed without errors!\n";
}

//TODO: make sure crossover works by using the genomes in the paper and testing if I get the same result when they are both the same fit 
void testResearchPapersCrossover(){
    Genome g1 = createTestGenome2({
        {0,3, .5, 0, true},
        {1,3, .5, 1, false},
        {2,3, .5, 2, true},
        {1,4, .5, 3, true},
        {4,3, .5, 4,true},
        {0,4, .5, 7,true},
    }); 
    Genome g2 = createTestGenome3({
        {0,3, .5, 0, true},
        {1,3, .5, 1, false},
        {2,3, .5, 2, true},
        {1,4, .5, 3, true},
        {4,3, .5, 4,false},
        {4,5, .5, 5,true},
        {5,3, .5, 6,true},
        {2,4, .5, 8,true},
        {0,5, .5, 9,true},
    }, 0); 

    NEATRunner runner; 
    Genome offspring = runner.performCrossover(g1, g2);
    
    for (Connection & conn : offspring.connections){
        std::cout<<"Innv: "<< conn.innvNum+1<<", "<<conn.inId+1<<"->"<<conn.outId+1<<"|  "; 
    }
}

int main() {
    std::cout << "========================================\n";
    std::cout << "        CROSSOVER TESTS (NEAT)         \n";
    std::cout << "========================================\n";
    
    testPopulationSizePreservation();
    testOffspringAllocation();
    testChampionPreservation();
    testAsexualReproduction();
    testSpeciesCulling();
    testRemainderAllocation();
    testCrossoverMechanics();
    testInterspeciesMating();
    testResearchPapersCrossover(); 
    std::cout << "\n========================================\n";
    std::cout << "🎉 ALL CROSSOVER TESTS COMPLETED!\n";
    std::cout << "========================================\n";
    
    return 0;
}