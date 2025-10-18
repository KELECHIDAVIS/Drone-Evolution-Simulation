#pragma once
#include "NeuralNetwork.hpp"
#include "Environment.hpp"
#include "helper.hpp"
#include <string>
#include <fstream>
#include <filesystem>


struct ReplayFrame{
    int frame; 
    int rocketRotation; 
    double rocketX; 
    double rocketY; 
    double targetX; 
    double  targetY; 

    nlohmann::json to_json() const {
        return nlohmann::json{
            {"frame", frame},
            {"rotation", rocketRotation},
            {"rocketX", rocketX},
            {"rocketY", rocketY},
            {"targetX", targetX},
            {"targetY", targetY},
        };
    }
}; 
class NEATRunner{
public:
    static constexpr int POP_SIZE = 150; // can go up to popsize = 1000 if needed  
    static constexpr int ENV_WIDTH = 400, ENV_HEIGHT=400; 
    static constexpr int SIM_LIFETIME =1000; // how many frames each genome gets  
    static constexpr double WEIGHT_MUTATION_RATE = 0.8;
    static constexpr double WEIGHT_PERTURB_CHANCE = 0.9;
    static constexpr double PERTURB_DELTA = 0.1;
    static constexpr double WEIGHT_MIN = -1.0;
    static constexpr double WEIGHT_MAX = 1.0;
    static constexpr double ADD_NODE_RATE = 0.03;
    static constexpr double ADD_LINK_RATE = 0.05; // use 0.3 for very large pops

    static constexpr float C1=1.0f, // how much weights excess genes have in differentiating species
    C2=1.0f, // how much weight disjoint genes have  
    C3=.4f, // how much weight weight differences have 
    COMP_THRESHOLD=3.0f; 
    int globalInnvNum=0;
    int genNum=0; 
    double totalAdjFit=0; // sum of the sumAdjFit for each species; used to calc proportion of population each species should have    
    double avgRawFit=0; 
    int gensSinceInnovation=0; 
    Species * bestPerformingSpecies = nullptr; // based off sum of adjusted fitness 
    Species * worstPerformingSpecies = nullptr; 
    double bestAdjFit ; // raw_fit/species_size; helps innovative genomes from worse species be more competitive 
    double  bestRawFit; 
    double  worstRawFit; 
    double  worstAdjFit; 
    
    std::vector<Genome> genomes ;
    std::vector<NeuralNetwork> networks; 
    std::vector<Environment> environments;  
    std::unordered_map<std::pair<int,int>, int, pair_hash> innvTracker ; // keep track of connections 
    std::vector<Species> speciesList;

    void clearDir(const std::filesystem::path &dir_path);

    void saveConstants(const std::filesystem::path &dir_path);

    /* initialization:
            all networks started fully connected in target xy, rocket xy, rocket xy vel as inputs and rocket angle and thrust as outputs
            (ALL VALUES WILL BE NORMALIZED: our activation function expects values from -1->1 )
        */
    NEATRunner(); 

    Genome initGenome(); 

    // creates a connection on a genome and adheres with the innovation tracker
    void createConnection(int in, int out , double weight, bool enabled, bool isRecurrent, Genome &genome); 

    Genome createGenome (int numInputs, int numBiases, int numOutputs, const std::vector<std::tuple<int, int, double, bool, int, bool>>& connections); 

    void runGeneration(); 

    // steps of NEAT: speciation, mutation, then crossover 

    double calcCompDistance(Genome& parent1, Genome& parent2);
    void keepTrackOfGenomeStats(Genome &genome);

    void speciate();
    void addConnectionMutation(Genome &genome);

    void addNodeMutation(Genome &genome);

    void perturbConnections(Genome &genome);

    void mutate();

    Genome &selectParentFromSpecies(Species &species);

    bool approxEqual(double a, double b, double epsilon);

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
    std::vector<ReplayFrame> evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env, bool replay); 
    double evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env); 
    // record the results for the generation 
    void saveGenerationResults(); 

}; 


