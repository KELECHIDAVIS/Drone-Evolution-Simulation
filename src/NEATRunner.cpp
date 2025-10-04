#include "NEATRunner.hpp"
#include <execution> // for parallelism

NEATRunner::NEATRunner()
{
    for (int i =0; i< POP_SIZE; i++){
        Genome genome = initGenome(); 
        genomes.push_back(genome); 

        // init their neural networks
        NeuralNetwork network(genome, TANH); 
        networks.push_back(network); 

        // init their environments 
        Environment env(ENV_WIDTH, ENV_HEIGHT); 
        environments.push_back(env); 
    }

    
}

Genome NEATRunner::initGenome() // at the start of the sim
{
    Genome genome; 
    
    genome.addNode(NodeType::INPUT);  // 0 - target relative x 
    genome.addNode(NodeType::INPUT);  // 1 - target relative y
    genome.addNode(NodeType::INPUT);  // 2 - rocket x vel
    genome.addNode(NodeType::INPUT);  // 3 - rocket y vel
    genome.addNode(NodeType::BIAS);   // 4 - bias
    genome.addNode(NodeType::OUTPUT); // 5 - rocket thrust
    genome.addNode(NodeType::OUTPUT); // 6 - rocket rotation
    
    // 50% chance per connection
    for (int in = 0; in <= 4; ++in) {
        for (int out = 5; out <= 6; ++out) {
            if (getRandNum(0, 1) < 0.5) {
                createConnection(in, out, getRandNum(-1, 1), true, false, genome);
            }
        }
    }


    return genome; 
}

void NEATRunner::createConnection(int in, int out, double weight, bool enabled, bool isRecurrent, Genome &genome)
{
    // if this connection alr exists just pull the innovation number 
    // otherwise store the current innv num, set connections innv num, then increment global num
    int innvNum ; 
    if (innvTracker.find({in, out})!=innvTracker.end()){
        innvNum = innvTracker[{in, out}]; 
    }else{
        innvTracker[{in, out}] = globalInnvNum; 
        innvNum = globalInnvNum; 
        globalInnvNum++;  
    }
    genome.addConnection(in, out, weight, enabled, innvNum,isRecurrent); 
}

void NEATRunner::runGeneration()
{
    testOutGenomes(); 

    speciate(); 

    mutate(); 

    crossover(); 

    saveGenerationResults(); 

    genNum++; 
}

// Single genome evaluation (non-parallel)
double NEATRunner::evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env) {
    env.reset();  // Make sure you have this method!
    net.reset();
    
    for (int step = 0; step < SIM_LIFETIME; step++) {
        Eigen::VectorXd input(4);
        input(0) = (env.target.pos(0) - env.rocket.pos(0)) / ENV_WIDTH;
        input(1) = (env.target.pos(1) - env.rocket.pos(1)) / ENV_HEIGHT;
        input(2) = env.rocket.vel(0) / Rocket::MAX_VEL;
        input(3) = env.rocket.vel(1) / Rocket::MAX_VEL;
        
        Eigen::VectorXd output = net.feedForward(input);
        env.rocket.setThrust(output(0));
        env.rocket.setRotation((int)(360 * output(1)));
        env.update(0.016f);
    }
    
    return env.score; //TODO: CHANGE FITNESS FUNCTION 
}

// Parallel wrapper
void NEATRunner::testOutGenomes() {
    std::vector<size_t> indices(genomes.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [this](size_t idx) {
            genomes[idx].fitness = evaluateGenome(
                genomes[idx], 
                networks[idx], 
                environments[idx]
            );
        });
}

/* speciation: The population is going to be split into different species based on compatability distance (delta = c1*E/N +c2*D/N + c3*W)
        where c1-3 are coefficients (hyperparemeters), E are numExcess and D is numDisjoint genes, N is the number of connection genes within Genome (can be set to 1 if <20),
        and W is avg weight differences of matching genes including disabled genes 

        the different coeffs allow us to adjust how important the three factors are when it comes to two genomes being from the same species 
*/
void NEATRunner::speciate()
{
    // run through each genome, compare it's compatibility to each species, decide its species; 
    // if there are no new species create one 
}

    
    

