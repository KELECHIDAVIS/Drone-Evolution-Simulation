#pragma once
#include "NeuralNetwork.hpp"
#include "Environment.hpp"
#include "helper.hpp"
class NEATRunner{
    static const int POP_SIZE = 150; // can go up to popsize = 1000 if needed  
    static const float C1, C2, C3, COMP_THRESHOLD; // TODO: all hyperparams are subject to change 
    int globalInnvNum=0;

public:
    std::vector<Genome> genomes ;
    std::vector<NeuralNetwork> networks; 
    std::vector<Environment> environments;  
    std::unordered_map<std::pair<int,int>, int, pair_hash> innvTracker ; // keep track of connections 
    
    /* initialization: 
        all networks started fully connected in target xy, rocket xy, rocket xy vel as inputs and rocket angle and thrust as outputs 
        (ALL VALUES WILL BE NORMALIZED: our activation function expects values from -1->1 )
    */
    NEATRunner(); 
    // steps of NEAT: speciation, mutation, then crossover 

    /* speciation: The population is going to be split into different species based on compatability distance (delta = c1*E/N +c2*D/N + c3*W)
        where c1-3 are coefficients (hyperparemeters), E are numExcess and D is numDisjoint genes, N is the number of connection genes within Genome (can be set to 1 if <20),
        and W is avg weight differences of matching genes including disabled genes 

        the different coeffs allow us to adjust how important the three factors are when it comes to two genomes being from the same species 
    */
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

    Genome initGenome(); 
}; 