#pragma once
#include "NeuralNetwork.hpp"
#include "Environment.hpp"
#include "helper.hpp"
class NEATRunner{
    static const int POP_SIZE = 150; // can go up to popsize = 1000 if needed  
    static const int ENV_WIDTH = 400, ENV_HEIGHT=400; 
    static const float C1=1.0, // how much weights excess genes have in differentiating species
     C2=1.0, // how much weight disjoint genes have  
     C3=.4, // how much weight weight differences have 
    COMP_THRESHOLD=3.0; // TODO: all hyperparams are subject to change, initially just what the paper had 
    int globalInnvNum=0;
    int genNum=0; 
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
    void createConnection(int in, int out , double weight, bool enabled, Genome &genome); 


    void runGeneration(); 

    // steps of NEAT: speciation, mutation, then crossover 

    
    void speciate(); 

    /* mutation: mutations will be stored according to there connections and innovNums (3->4: 12)
    Mutation chances are determined by hyperparams, initially:
    The champion of each species with more the 5 members remains unchanged 
    Each Genome has an 80% chance of getting their connection weights mutated: 
        In which case each weight has a 90% chance of being uniformly perturbed 
        And A 10% chance of being assigned a new random value 
    */
    void mutate(); 

    /*crossover: 
    FOR EACH NEW CONNECTION UPDATE GLOBAL INNOVATION NUM
    There is a 75% chance that and a gene will be disabled if it was disabled in either parent
    Interspecies mating rate is .001
    For smaller populations: 
        new node chance: .03
        new link chance: .05
    For Larger populations: 
        new node chance: .03
        new link chance: .3  // because larger pops can tolerate a larger number of prospective species and greater diversity
    */
    void crossover();
    
    // this is going to make use of parallelism to test out genomes efficiently
    void testOutGenomes(); 

    // record the results for the generation 
    void saveGenerationResults(); 

}; 