#pragma once
#include "NeuralNetwork.hpp"
#include "Environment.hpp"
#include "helper.hpp"
#include <string>
#include <fstream>
#include <filesystem>
#include <cmath> 

// fix memory issues with the genFrame array; each frame may be too big of an object
//TODO: make the data types the most memory efficient to save space 
struct ReplayFrame{
    unsigned short frame; //0 -> sim_lifetime
    unsigned short rocketRotation; //0 -> 360 
    float rocketThrust; // 0->1
    double rocketX;  
    double rocketY; 
    double targetX; 
    double  targetY; 
    std::pair<float,float> vertex1;
    std::pair<float,float> vertex2;
    std::pair<float, float> vertex3;
    bool valid = false; // init to false to replay purposes 
    nlohmann::json to_json() const {
        nlohmann::json frame_json{
            {"frame", frame},
            {"rotation", rocketRotation},
            {"thrust",rocketThrust},
            {"rocketX", rocketX},
            {"rocketY", rocketY},
            {"targetX", targetX},
            {"targetY", targetY},
            {"v1x", vertex1.first},
            {"v1y", vertex1.second},
            {"v2x", vertex2.first},
            {"v2y", vertex2.second},
            {"v3x", vertex3.first},
            {"v3y", vertex3.second}
        };

        
        return frame_json;
    }
}; 
class NEATRunner{
public:
    #ifdef NDEBUG
        // Release build
        static constexpr int GENS = 1000; // release can handles much more generations much faster (1000+)
    #else
        // Debug build
        static constexpr int GENS = 1;
    #endif
    static constexpr int POP_SIZE = 200; // can go up to popsize = 1000 if needed  
    static constexpr int ENV_WIDTH = 640, ENV_HEIGHT=640; 
    static constexpr int SIM_LIFETIME =500; // how many frames each genome gets  
    static constexpr int STAGNATION_THRESHOLD = 15 ; // species are considered stagnant after this amt of gens without improvement in their best fitness
    static constexpr float INIT_CONNECTIVITY_RATE= .90; // originally .5
    static constexpr double WEIGHT_MUTATION_RATE = 0.8;
    static constexpr double WEIGHT_PERTURB_CHANCE = 0.9;
    static constexpr double PERTURB_DELTA = 0.25;
    static constexpr double WEIGHT_MIN = -1.0;
    static constexpr double WEIGHT_MAX = 1.0;
    static constexpr double ADD_NODE_RATE = .05f; //0.03;
    static constexpr double ADD_LINK_RATE = 0.2f; // .05 // use 0.3 for very large pops
    static constexpr float C1=1.0f, // how much weights excess genes have in differentiating species
    C2=1.0f, // how much weight disjoint genes have  
    C3= 2.f, //.4f, // how much weight weight differences have ; for larger pops should be increased 
    COMP_THRESHOLD=3.0f; // should change for larger pops  
    static constexpr float HITS_FIT_MULTIPLIER= 120, TIME_EFFICIENCY_FIT_MULTIPLIER =20, DISTANCE_FIT_MULTIPLIER=20, TIME_LIVED_MULTIPLIER =100; 
    double MAX_TARGET_DIST = std::sqrt( ENV_HEIGHT*ENV_HEIGHT + ENV_WIDTH*ENV_WIDTH); 
    int globalInnvNum=0;
    int genNum=0; 
    double totalAdjFit=0; // sum of the sumAdjFit for each species; used to calc proportion of population each species should have    
    double avgRawFit=0; 
    int gensSinceInnovation=0; 
    Species * bestPerformingSpecies = nullptr; // based off sum of adjusted fitness 
    Species * worstPerformingSpecies = nullptr; 
    Genome bestPerformingGenome;
    double bestAdjFit ; // raw_fit/species_size; helps innovative genomes from worse species be more competitive 
    double  bestRawFit; 
    double  worstRawFit; 
    double  worstAdjFit; 
    std::vector<Genome> genomes ;
    std::vector<NeuralNetwork> networks; 
    std::vector<Environment> environments;  
    std::unordered_map<std::pair<int,int>, int, pair_hash> innvTracker ; // keep track of connections 
    std::vector<Species> speciesList;
    std::vector<std::vector<ReplayFrame>> genFrames; // [POPSIZE][SIMLIFETIME] 
    size_t bestReplayIndex ; // the genome that has the best replay 


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
    void initNetworksForGenomes(); 

    double evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env, int genomeIndx);
    // record the results for the generation 
    void saveGenerationResults(); 

}; 


