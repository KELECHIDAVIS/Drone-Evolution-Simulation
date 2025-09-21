#include "NEATRunner.hpp"

NEATRunner::NEATRunner()
{
    for (int i =0; i< POP_SIZE; i++){
        Genome genome = initGenome(); 
        genomes.push_back(genome); 

        // init their neural networks
        NeuralNetwork network(genome); 
        networks.push_back(network); 

        // init their environments 
        Environment env(ENV_WIDTH, ENV_HEIGHT); 
        environments.push_back(env); 
    }

    
}

Genome NEATRunner::initGenome() 
{
    Genome genome ; 
    
    genome.addNode(NodeType::INPUT); // 0 
    genome.addNode(NodeType::INPUT); // 1 
    genome.addNode(NodeType::INPUT); //2
    genome.addNode(NodeType::INPUT); //3
    genome.addNode(NodeType::INPUT); //4
    genome.addNode(NodeType::INPUT); //5
    genome.addNode(NodeType::OUTPUT); //6
    genome.addNode(NodeType::OUTPUT); //7

    // Fully connect all inputs [0..5] to all outputs [6..7]
    for (int in = 0; in <= 5; ++in) {
        for (int out = 6; out <= 7; ++out) {
        createConnection(in, out, getRandNum(-1, 1), true, genome);
    }
}

    return genome; 
}

void NEATRunner::createConnection(int in, int out, double weight, bool enabled, Genome &genome)
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
    genome.addConnection(in, out, weight, enabled, innvNum); 
}

void NEATRunner::runGeneration()
{
    speciate(); 

    mutate(); 

    crossover(); 

    testOutGenomes(); 

    saveGenerationResults(); 

    genNum++; 
}

/* speciation: The population is going to be split into different species based on compatability distance (delta = c1*E/N +c2*D/N + c3*W)
        where c1-3 are coefficients (hyperparemeters), E are numExcess and D is numDisjoint genes, N is the number of connection genes within Genome (can be set to 1 if <20),
        and W is avg weight differences of matching genes including disabled genes 

        the different coeffs allow us to adjust how important the three factors are when it comes to two genomes being from the same species 
*/
void NEATRunner::speciate()
{
    
}
