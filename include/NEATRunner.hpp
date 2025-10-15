#pragma once
#include "NeuralNetwork.hpp"
#include "Environment.hpp"
#include "helper.hpp"
class NEATRunner{
    static const int POP_SIZE = 150; // can go up to popsize = 1000 if needed  
    static const int ENV_WIDTH = 400, ENV_HEIGHT=400; 
    static const int SIM_LIFETIME =1000; // each genome gets 1000 frames 
    static constexpr float C1=1.0f, // how much weights excess genes have in differentiating species
     C2=1.0f, // how much weight disjoint genes have  
     C3=.4f, // how much weight weight differences have 
    COMP_THRESHOLD=3.0f; 
    int globalInnvNum=0;
    int genNum=0; 
    int totalAdjFit; // sum of the sumAdjFit for each species; used to calc proportion of population each species should have    
    Species * bestPerformingSpecies = nullptr; 
public:
    std::vector<Genome> genomes ;
    std::vector<NeuralNetwork> networks; 
    std::vector<Environment> environments;  
    std::unordered_map<std::pair<int,int>, int, pair_hash> innvTracker ; // keep track of connections 
    std::vector<Species> speciesList; 
    
    /* initialization: 
        all networks started fully connected in target xy, rocket xy, rocket xy vel as inputs and rocket angle and thrust as outputs 
        (ALL VALUES WILL BE NORMALIZED: our activation function expects values from -1->1 )
    */
    NEATRunner(); 

    Genome initGenome(); 

    // creates a connection on a genome and adheres with the innovation tracker
    void createConnection(int in, int out , double weight, bool enabled, bool isRecurrent, Genome &genome); 


    void runGeneration(); 

    // steps of NEAT: speciation, mutation, then crossover 

    double calcCompDistance(Genome& parent1, Genome& parent2); 
    
    void speciate(); 


    void mutate();

    Genome &selectParentFromSpecies(Species &species);

    Genome performCrossover(Genome &parent1, Genome &parent2);

    /*crossover:
        FOR EACH NEW CONNECTION UPDATE GLOBAL INNOVATION NUM
        There is a 75% chance that and a gene will be disabled if it was disabled in either parent
        Interspecies mating rate is .001
        */
    Species &selectRandomSpecies();
    void crossover();

    // this is going to make use of parallelism to test out genomes efficiently
    void testOutGenomes(); 
    double evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env); 
    // record the results for the generation 
    void saveGenerationResults(); 

}; 