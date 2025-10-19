#include "NEATRunner.hpp"
#include <execution> // for parallelism

void NEATRunner::clearDir(const std::filesystem::path& dir_path){
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
        std::filesystem::remove_all(entry.path());
}
void NEATRunner::saveConstants(const std::filesystem::path& dir_path){
    nlohmann::json constants; 
    constants["POP_SIZE"] = POP_SIZE; 
    constants["ENV_WIDTH"] = ENV_WIDTH; 
    constants["ENV_HEIGHT"] = ENV_HEIGHT; 
    constants["SIM_LIFETIME"] = SIM_LIFETIME; 
    constants["WEIGHT_MUTATION_RATE"] = WEIGHT_MUTATION_RATE; 
    constants["WEIGHT_PERTURB_CHANCE"] = WEIGHT_PERTURB_CHANCE; 
    constants["PERTURB_DELTA"] = PERTURB_DELTA; 
    constants["WEIGHT_MIN"] = WEIGHT_MIN; 
    constants["WEIGHT_MAX"] = WEIGHT_MAX; 
    constants["ADD_NODE_RATE"] = ADD_NODE_RATE; 
    constants["ADD_LINK_RATE"] = ADD_LINK_RATE; 
    constants["C1"] = C1; 
    constants["C2"] = C2; 
    constants["C3"] = C3; 
    constants["COMP_THRESHOLD"] = COMP_THRESHOLD; 

    constants ["TARGET_RADIUS"] = Target::RADIUS; 
    constants["ROCKET_BASE"] = Rocket::BASE; 
    constants["ROCKET_HEIGHT"] = Rocket::HEIGHT;
    constants["ROCKET_MAX_THRUST"] = Rocket::MAX_THRUST;
    constants["ROCKET_MAX_VEL"] = Rocket::MAX_VEL;
    constants["ROCKET_GRAVITY"] = Rocket::GRAVITY;

    std::ofstream outFile("simulation_data/constants.json"); 
    outFile << constants.dump(); 
    outFile.close();
}
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


    // Create data directory if it doesn't exist
    const std::filesystem::path dir = "simulation_data"; 

    std::filesystem::create_directories(dir.string());

    // delete all previous files   
    clearDir(dir); 
    // make sure constants are written into file as well
    saveConstants(dir); 
    
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
                createConnection(in, out, getRandNum(WEIGHT_MIN, WEIGHT_MAX), true, false, genome);
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
    }else{ // innovation was found 
        innvTracker[{in, out}] = globalInnvNum; 
        innvNum = globalInnvNum; 
        globalInnvNum++;  
        gensSinceInnovation=0; 
    }
    genome.addConnection(in, out, weight, enabled, innvNum,isRecurrent); 
}

Genome NEATRunner::createGenome(int numInputs, int numBiases, int numOutputs, const std::vector<std::tuple<int, int, double, bool, int, bool>> &connections)
{
    Genome genome;
    
    for(int i = 0; i< numInputs; i++){
        genome.addNode(NodeType::INPUT);   
    }

    for(int i = 0; i< numBiases; i++){
        genome.addNode(NodeType::BIAS);   
    }
    
    for(int i = 0; i< numOutputs; i++){
        genome.addNode(NodeType::OUTPUT);   
    }
    
    
    for (const auto& [in, out, weight, enabled, innov, recurrent ] : connections) {
        genome.addConnection(in, out, weight, enabled, innov, recurrent);
    }
    
    return genome;
}

void NEATRunner::runGeneration()
{
    gensSinceInnovation+=1 ; 

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

    int hitCount = 0;
    int totalFrames = 0;

    bool alive= true; 
    for (int step = 0; step < SIM_LIFETIME; step++) {

        if(!alive) 
            break; 

        Eigen::VectorXd input(4);
        input(0) = (env.target.pos(0) - env.rocket.pos(0)) / ENV_WIDTH;
        input(1) = (env.target.pos(1) - env.rocket.pos(1)) / ENV_HEIGHT;
        input(2) = env.rocket.vel(0) / Rocket::MAX_VEL;
        input(3) = env.rocket.vel(1) / Rocket::MAX_VEL;
        
        Eigen::VectorXd output = net.feedForward(input);
        env.rocket.setThrust(output(0));
        env.rocket.setRotation((int)(360 * output(1)));

        alive = env.update(0.016f);

    }
    return env.score; //TODO: CHANGE FITNESS FUNCTION;
}
std::vector<ReplayFrame> NEATRunner::evaluateGenome(Genome &genome, NeuralNetwork &net, Environment &env, bool replay) {
    env.reset();  
    net.reset();
    
    bool alive = true; 
    std::vector<ReplayFrame> frames; 

    for (int step = 0; step < SIM_LIFETIME; step++) {
        if(!alive)
            break; 

        if(replay){
            ReplayFrame frame = {
                step,
                env.rocket.getRotation(),
                env.rocket.getThrust(),
                env.rocket.pos(0),
                env.rocket.pos(1),
                env.target.pos(0),
                env.target.pos(1),
            };
            frames.push_back(frame); 
        }
        Eigen::VectorXd input(4);
        input(0) = (env.target.pos(0) - env.rocket.pos(0)) / ENV_WIDTH;
        input(1) = (env.target.pos(1) - env.rocket.pos(1)) / ENV_HEIGHT;
        input(2) = env.rocket.vel(0) / Rocket::MAX_VEL;
        input(3) = env.rocket.vel(1) / Rocket::MAX_VEL;
        
        Eigen::VectorXd output = net.feedForward(input);
        env.rocket.setThrust(output(0));
        env.rocket.setRotation((int)(360 * output(1)));
        alive = env.update(0.016f);
    }
    return frames; 
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
    nlohmann::json gen_json; 
    gen_json["generation"] = genNum;
    for (const auto& species : speciesList) {
        nlohmann::json species_json;
        species_json["id"] = species.id;
        species_json["bestFitness"] = species.bestFitness;
        for (const auto& genome : species.members) {
            nlohmann::json genome_json= genome.to_json(); 
            species_json["members"].push_back(genome_json); // members are sorted based on performance already
        }
        gen_json["species"].push_back(species_json);
    }
    gen_json["bestRawFit"] = bestRawFit; 
    gen_json["worstRawFit"] = worstRawFit; 
    gen_json["bestAdjFit"] = bestAdjFit; 
    gen_json["worstAdjFit"] = worstAdjFit; 
    gen_json["avgRawFit"] = avgRawFit; 

    gen_json["bestSpecies"] = bestPerformingSpecies->id; 
    gen_json["worstSpecies"] = worstPerformingSpecies->id; 
    
    gen_json["gensSinceInnovation"] = gensSinceInnovation; 
    
    // replay the champ
    // the best performer from the best species 
    Genome champ = bestPerformingSpecies->members.front();
    Environment replayEnv(ENV_WIDTH, ENV_HEIGHT);
    NeuralNetwork replayNet(champ, TANH);

    std::vector<ReplayFrame> championReplayFrames = evaluateGenome(
        champ, 
        replayNet, 
        replayEnv, 
        true
    ); 
    
    for (const ReplayFrame& frame : championReplayFrames) {
        gen_json["champReplayFrames"].push_back(frame.to_json()); 
    }

    std::ofstream outFile("simulation_data/gen_"+std::to_string(genNum)+".json"); 
    outFile << gen_json.dump(); 
    outFile.close(); 
    
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

bool NEATRunner::approxEqual(double a, double b, double epsilon = 1e-6) {
    return std::abs(a - b) < epsilon;
}
// THE CONNECTIONS ARE ASSUMED TO BE SORTED 
Genome NEATRunner::performCrossover(Genome& parent1, Genome& parent2 ){
    Genome offspring;
    
    // Determine which parent is more fit
    bool parent1Fitter = parent1.fitness > parent2.fitness;
    bool equalFitness = approxEqual(parent1.fitness, parent2.fitness);
        
    // Sort connections by innovation number
    auto p1Conns = parent1.connections;
    auto p2Conns = parent2.connections;
    
    
    int i = 0, j = 0;
    
    // Crossover connections
    while(i < p1Conns.size() || j < p2Conns.size()) {
        Connection* conn1 = (i < p1Conns.size()) ? &p1Conns[i] : nullptr;
        Connection* conn2 = (j < p2Conns.size()) ? &p2Conns[j] : nullptr;
        
        Connection inheritedConn;
        bool inherit = false;
        
        // Both parents have this gene (matching)
        if(conn1 && conn2 && conn1->innvNum == conn2->innvNum) {
            // Randomly inherit from either parent
            inheritedConn = (getRandNum(0, 1) < 0.5) ? *conn1 : *conn2;
            
            // 75% chance to disable if disabled in either parent
            if(!conn1->isEnabled || !conn2->isEnabled) {
                if(getRandNum(0, 1) < 0.75) {
                    inheritedConn.isEnabled = false;
                }
            }
            inherit = true;
            i++; j++;
        }
        // Disjoint/excess gene
        else {
            if(equalFitness) {
                // if the fitnesses are equal inherit all disjoint and excess
                if(conn1 && (!conn2 || conn1->innvNum < conn2->innvNum)) {
                    inheritedConn = *conn1;
                    inherit = true;
                    i++;
                } else {
                    inheritedConn = *conn2;
                    inherit = true;
                    j++;
                }
            } else {
                // Inherit from fitter parent
                if(parent1Fitter) {
                    if(conn1 && (!conn2 || conn1->innvNum < conn2->innvNum)) {
                        inheritedConn = *conn1;
                        inherit = true;
                        i++;
                    } else {
                        j++;
                    }
                } else {
                    if(conn2 && (!conn1 || conn2->innvNum < conn1->innvNum)) {
                        inheritedConn = *conn2;
                        inherit = true;
                        j++;
                    } else {
                        i++;
                    }
                }
            }
        }
        
        if(inherit) {
            offspring.connections.push_back(inheritedConn);
        }
    }
    
    // Now add all required nodes from both parents
    // both parents should have the same bias, input, and output nodes. the differring nodes are 
    //nodes unlike innvNums should be sequential and range from id = 0 to nodes.size() 
    std::unordered_map<int, Node> allNodes;
    int parentMax= INT_MIN, parentMin = INT_MAX; 
    for(const Node& node : parent1.nodes) {
        allNodes[node.id] = node;
        parentMax = std::max(parentMax , node.id); 
        parentMin = std::min(parentMin , node.id); 
    }
    for(const Node& node : parent2.nodes) {
        allNodes[node.id] = node;
        parentMax = std::max(parentMax , node.id); 
        parentMin = std::min(parentMin , node.id); 
    }
    
    for(int i =parentMin; i<= parentMax; i++){
        offspring.addNode(allNodes[i].type, i); 
    }
    
    return offspring;
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

    for(int i = spotsLeft; i>0; i--){
        Genome& parent1 = selectParentFromSpecies(*bestPerformingSpecies);
        Genome& parent2 = selectParentFromSpecies(*bestPerformingSpecies);
        Genome offspring = performCrossover(parent1, parent2);
        nextGen.push_back(offspring); 
    }

    // set current gen = to next gen 
    genomes = nextGen; 
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
    //if (N < 20) N = 1.0;  // normalization from paper
    if(numMatchingWeights>0) avgWeightDiff/=numMatchingWeights; 

    return C1*numExcess/N + C2*numDisjoint/N + C3*avgWeightDiff; 
}

void NEATRunner::keepTrackOfGenomeStats(Genome &genome){
    if(bestRawFit == 0.0 || genome.fitness > bestRawFit){
        bestRawFit = genome.fitness; 
    }    
    if(worstRawFit == 0.0 || genome.fitness < worstRawFit){
        worstRawFit = genome.fitness; 
    }    
    if(bestAdjFit == 0.0 || genome.adjustedFitness > bestAdjFit){
        bestAdjFit = genome.adjustedFitness; 
    }    
    if(worstAdjFit == 0.0 || genome.adjustedFitness < worstAdjFit){
        worstAdjFit = genome.adjustedFitness; 
    }       
}

void NEATRunner::speciate()
{

    // Reset stats for this generation
    bestRawFit = -std::numeric_limits<double>::infinity();
    worstRawFit = std::numeric_limits<double>::infinity();
    bestAdjFit = -std::numeric_limits<double>::infinity();
    worstAdjFit = std::numeric_limits<double>::infinity();

    bestPerformingSpecies = nullptr; 
    worstPerformingSpecies = nullptr; 
    // run through each genome, compare it's compatibility to each species, decide its species; 
    // if there are no new species create one 
    // keep track of the stats of the genomes 
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
    avgRawFit = 0; 
    // again for each species set their adjust fitness based on the size of their species 
    for (Species &species: speciesList){
        species.bestFitness = INT_MIN; 
        species.sumOfAdjFits = 0; 
        for(Genome& genome : species.members){
            genome.adjustedFitness = genome.fitness/species.members.size(); 
            species.bestFitness = std::max(genome.fitness, species.bestFitness) ; 
            species.sumOfAdjFits += genome.adjustedFitness; 
            avgRawFit+=genome.fitness; 
            keepTrackOfGenomeStats(genome); 
        }
        
        totalAdjFit += species.sumOfAdjFits; 
        
        if (!bestPerformingSpecies || species.sumOfAdjFits > bestPerformingSpecies->sumOfAdjFits)
            bestPerformingSpecies = &species;  
        
        if (!worstPerformingSpecies || species.sumOfAdjFits < worstPerformingSpecies->sumOfAdjFits)
            worstPerformingSpecies = &species;  

        
    }

    avgRawFit/= POP_SIZE; 

    if (totalAdjFit <= 0.0) {
        std::cerr << "!! totalAdjFit was zero, setting totalAdjFit = 1 .\n";
        totalAdjFit = 1.0;
    }

}

/* 3.1 Genetic Encoding (pg 107)
Mutations can change connection weights or network's structure 
Setting connections to a new random value
Perturbing connections 
Structural Mutation:
Adding new node 
Adding new connection
*/


// connects two previously un connected nodes with a new random connection  
void NEATRunner::addConnectionMutation(Genome &genome){
    //if genome only has one node return 
    if(genome.nodes.size() <=1 ) return;  
    // create map for each node that shows the connections it has to other nodes 
    std::unordered_map<int, std::unordered_set<int>> existingConnections; 
    std::vector<int> nodeIndices ; 
    // init map for every node;  nodes should be 0->genome.nodes.size() 
    for(Node & node: genome.nodes){
        existingConnections[node.id];
        nodeIndices.push_back(node.id);  
    }
    for(Connection & conn: genome.connections){
        existingConnections[conn.inId].insert(conn.outId); 
    } 

    // pick random node as input
    int randIndx = floor(getRandNum(0, nodeIndices.size()));  
    int inId = nodeIndices[randIndx];
    nodeIndices.erase(nodeIndices.begin() + randIndx);
    int outId = -1 ; 

    // go through the rest of the list until you find a node the inId isn't connected to 
    for(int i= 0; i< nodeIndices.size(); i++){
        if(existingConnections[inId].find(nodeIndices[i]) != existingConnections[inId].end())
            continue; 
        // save for out id  
        outId= nodeIndices[i]; 
    }

    if (outId != -1){ // create new connection 
        // first figure out if the connection would be recurrent 
        // if out node has outgoing connection to inNode it's recurrent 
        bool isReccurrent = existingConnections[outId].find(inId) != existingConnections[outId].end(); 
        createConnection(inId, outId, getRandNum(WEIGHT_MIN, WEIGHT_MAX), true, isReccurrent, genome); 
    }
}
// Existing connection is split and a new node is add in between 
void NEATRunner::addNodeMutation(Genome &genome){
    std::vector<int> indices(genome.connections.size()); 
    std::iota(indices.begin(), indices.end(), 0 ); 
    std::shuffle(indices.begin(), indices.end(), getRNG());
    
    for(int indx : indices){
        Connection conn = genome.connections[indx]; 

        if(!conn.isEnabled) continue; 

        genome.addNode(NodeType::HIDDEN); 
        Node &newNode = genome.nodes.back();

        // Find and disable the original connection (it might have moved due to reallocation)
        for (auto &c : genome.connections)
        {
            if (c.inId == conn.inId && c.outId == conn.outId && c.innvNum == conn.innvNum)
            {
                c.isEnabled = false;
                break;
            }
        }

        createConnection(conn.inId, newNode.id, 1.0, true, conn.isRecurrent, genome); 
        createConnection(newNode.id, conn.outId, conn.weight, true, conn.isRecurrent, genome); 

        break; 

    }

}
// each weight has a 90% chance of being uniformly perturbed and a 10% chance of being assigned new random val 
void NEATRunner::perturbConnections(Genome &genome){
    for(Connection & conn: genome.connections){
        if(getRandNum(0,1)< WEIGHT_PERTURB_CHANCE){
            double delta = getRandNum(-.2, 2 ); 
            conn.weight += delta; 
            conn.weight = std::clamp(conn.weight, WEIGHT_MIN, WEIGHT_MAX); // keep within range
        }else{
            conn.weight = getRandNum(WEIGHT_MIN,WEIGHT_MAX); 
        }
    }
}
void NEATRunner::mutate()
{   
    // make sure the champ of each species isn't affected
    // if a genome's raw fitness == the best fitness of its species && there champ hasn't been selected yet then it is the champ of that species
    // we have to go through each genome instead of each species since newly created offspring don't have a set species yet 
    std::unordered_set<int>  champSelected; // keep track if champ has been selected for a certain speciesid 
    
    for (Genome &genome: genomes){
        // check champ 
        if(genome.speciesID >=0){
            Species &species = speciesList[genome.speciesID]; 
            
            if (species.members.size()>=5 && champSelected.find(species.id) == champSelected.end() && approxEqual(species.bestFitness ,genome.fitness)){
                champSelected.insert(species.id); 
                continue ; // do not mutate this genome 
            }

            if (getRandNum(0, 1) < WEIGHT_MUTATION_RATE){
                perturbConnections(genome); 
            }

            if(getRandNum(0,1 ) < ADD_LINK_RATE){
                addConnectionMutation(genome); 
            }

            if(getRandNum(0,1 )< ADD_NODE_RATE){
                addNodeMutation(genome); 
            }
        }
    }

}
