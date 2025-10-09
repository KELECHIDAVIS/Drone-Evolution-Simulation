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

    crossover(); 

    mutate(); 

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

void NEATRunner::saveGenerationResults()
{
}

/*
Every species is assigned a diff number of offspring in proportion to the sum of adjusted fitnesses of its members
Champ of each species with more than 5 members gets copied unchanged 
Species then reproduces by first eliminating lowest performers (lower performers have a higer likelihood of getting replaced to protect innovation)
cross over happens within species with a .001 chance for interspecies mating
Then replace the spaces with offspring
*/
// uses fitness proportionate selection; The higher the fitness the greater chance to be picked 
Genome& NEATRunner::selectParentFromSpecies(Species& species) {
    // Calculate total fitness
    double totalFitness = 0;
    for(Genome& g : species.members) {
        totalFitness += g.fitness;
    }
    
    // Roulette wheel selection
    double random = getRandNum(0, totalFitness);
    double cumulative = 0;
    for(Genome& g : species.members) {
        cumulative += g.fitness;
        if(cumulative >= random) {
            return g;
        }
    }
    
    return species.members.front(); // fallback
}
Genome NEATRunner::performCrossover(Genome& parent1, Genome& parent2 ){
    // Matching genes are inherited randomly, disjoint and excess genes are inherited from most fit parent 
    // make the calc comp distance function return 
}
Species& NEATRunner::selectRandomSpecies(){
    int indx = std::floor(getRandNum(0, speciesList.size())); 
    return speciesList[indx]; 
}
void NEATRunner::crossover()
{
    std::vector<Genome> nextGen; 
    int spotsLeft = POP_SIZE; 
    for(Species& species: speciesList){
        // calc what proportion of the population their offspring should make up in the next generation 
        float proportion= species.sumOfAdjFits/((float) totalAdjFit); 
        int numOffSpring= floor(proportion*POP_SIZE); //round down 
        
        // if the case happens that it's greater than the max, set to it 
        if (numOffSpring > spotsLeft) numOffSpring = spotsLeft; 

        spotsLeft -= numOffSpring; 

        // TODO: will just sort for rn but could use adj fit as an average and if genome < speciesAdj it gets deleted  
        // remove badly performing members from a species; greater fits are at front 
        std::sort(species.members.begin(), species.members.end(), 
        [](const Genome& a, const Genome& b) { return a.fitness > b.fitness; });


        int newSize = std::ceil(species.members.size()/2.0) ; // remove bottom half 
        species.members.resize(newSize); 

        
        for ( int j=0; j< numOffSpring; j++){
            Genome offspring; 
            // Champion preservation (first offspring if species > 5)
            if(j == 0 && species.members.size() > 5) {
                offspring = species.members[0]; // best genome, unchanged
            }
            // Single member - asexual reproduction
            else if(species.members.size() == 1) {
                offspring = species.members[0];
            }
            // Mutation only (25% of offspring)
            else if(getRandNum(0, 1) < 0.25) {
                Genome& parent = selectParentFromSpecies(species);
                offspring = parent;
            }
            // Crossover (75% of offspring)
            else {
                if(getRandNum(0, 1) < 0.001) {
                    // Interspecies mating
                    Genome& parent1 = selectParentFromSpecies(species);
                    Species& otherSpecies = selectRandomSpecies();
                    Genome& parent2 = selectParentFromSpecies(otherSpecies);
                    offspring = performCrossover(parent1, parent2);
                } else {
                    // Within-species mating
                    Genome& parent1 = selectParentFromSpecies(species);
                    Genome& parent2 = selectParentFromSpecies(species);
                    offspring = performCrossover(parent1, parent2);
                }
            }
            
            nextGen.push_back(offspring);
        }



    }
    // if there are left over spots give to the best species 

    // add the genomes to their species 
}


/*
Genes that don't match are either excess (within other parents innv range), disjoint (outside range) 
speciation: The population is going to be split into different species based on compatability distance (delta = c1*E/N +c2*D/N + c3*W)
        where c1-3 are coefficients (hyperparemeters), E are numExcess and D is numDisjoint genes, N is the number of connection genes within larger Genome (can be set to 1 if <20),
        and W is avg weight differences of matching genes including disabled genes 

        the different coeffs allow us to adjust how important the three factors are when it comes to two genomes being from the same species 
*/
// CONNECTION GENES ARE ASSUMED TO BE SORTED
double NEATRunner::calcCompDistance(Genome& parent1, Genome& parent2){
    int numExcess =0; 
    int numDisjoint= 0; 
    double avgWeightDiff= 0; 

    int numMatchingWeights=0; 
    int firstIt=0, secondIt=0; 
    int parent1Size = parent1.connections.size(); 
    int parent2Size = parent2.connections.size(); 
    std::pair<int, int> parent1InnovRange= {INT_MAX, INT_MIN}; 
    std::pair<int, int> parent2InnovRange= {INT_MAX, INT_MIN}; 


    for(Connection &conn: parent1.connections){
        parent1InnovRange.first = std::min(conn.innvNum ,parent1InnovRange.first ); 
        parent1InnovRange.second = std::max(conn.innvNum ,parent1InnovRange.second ); 
    }
    for(Connection &conn: parent2.connections){
        parent2InnovRange.first = std::min(conn.innvNum ,parent2InnovRange.first ); 
        parent2InnovRange.second = std::max(conn.innvNum ,parent2InnovRange.second ); 
    }

    while(firstIt<parent1Size &&secondIt<parent2Size)
    {
        // if both current connections have same inv num continue
        Connection& conn1 = parent1.connections[firstIt]; 
        Connection& conn2 = parent2.connections[secondIt]; 
        if(conn1.innvNum == conn2.innvNum){
            firstIt++; 
            secondIt++; 
            // calc avg weight diff
            avgWeightDiff+= std::abs(conn1.weight - conn2.weight)   ; 
            numMatchingWeights++; 
        }else if (conn1.innvNum > conn2.innvNum){
            // if conn2 within range of parent1 connections its disjoint
            if (conn2.innvNum > parent1InnovRange.first ){ // if greater than it min innov its within range
                numDisjoint++; 
            }else{
                numExcess++; 
            }
            secondIt++; 
        }else{ // conn2.innovNum > conn1.innovNum
            if (conn1.innvNum > parent2InnovRange.first){
                numDisjoint++; 
            }else{
                numExcess++; 
            }
            firstIt++; 
        }

    }
    // now anything left over is excess; since one is zero just add 
    numExcess += (parent1Size-firstIt)+(parent2Size-secondIt); 

    //(delta = c1*E/N +c2*D/N + c3*W)
    double N = std::max(parent1Size, parent2Size); 
    if (N < 20) N = 1.0;  // normalization from paper
    if(numMatchingWeights>0) avgWeightDiff/=numMatchingWeights; 

    return C1*numExcess/N + C2*numDisjoint/N + C3*avgWeightDiff; 
}
void NEATRunner::speciate()
{

    // run through each genome, compare it's compatibility to each species, decide its species; 
    // if there are no new species create one 
    for(Genome &genome: genomes){

        // SORT CONNECTIONS IN GENOME TO MAKE SPECIATION EASIER
        std::sort(genome.connections.begin(), genome.connections.end(), 
        [](const Connection& a, const Connection& b) { return a.innvNum < b.innvNum; });

        bool foundMatch = false; 

        for(Species &species : speciesList){
            // calc compatability 
            //select rand genome to be rep if size > 1 
            int numMembers= species.members.size(); 
            int randIndx = (numMembers == 1)  ? 0  :  floor(getRandNum(0, numMembers)); 
            double compDist = calcCompDistance(genome, species.members[randIndx]);
            
            // if within threshold genome belongs to species
            if(compDist < COMP_THRESHOLD){
                genome.speciesID = species.id; 
                species.members.push_back(genome); 
                foundMatch= true;
                break; // stop looking through species 
            } 
        }

        if(!foundMatch){ // create new species w/ this genome as its rep
            Species newSpecies; 
            newSpecies.id=speciesList.size(); 
            genome.speciesID= newSpecies.id; 
            newSpecies.bestFitness = genome.fitness; 
            newSpecies.appearedInGen= genNum; 
            newSpecies.members.push_back(genome); 
            speciesList.push_back(newSpecies); 
        }
    }

    
    // sum total adj fit for all species within population
    totalAdjFit= 0; 

    // again for each species set their adjust fitness based on the size of their species 
    for (Species &species: speciesList){
        species.bestFitness = INT_MIN; 
        species.sumOfAdjFits = 0; 
        for(Genome& genome : species.members){
            genome.adjustedFitness = genome.fitness/species.members.size(); 
            species.bestFitness = std::max(genome.fitness, species.bestFitness) ; 
            species.sumOfAdjFits += genome.adjustedFitness; 
        }
        totalAdjFit+=species.sumOfAdjFits; 
        // TODO: store the species with the best adjusted fitness for excess 
    }

}

void NEATRunner::mutate()
{
}
