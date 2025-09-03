// an agent has an environment (target and rocket), has a genome, and a neural network 
#include "Genome.hpp"; 
#include "NeuralNetwork.hpp"; 
#include "Environment.hpp"; 

struct Agent{
    Genome& genome; 
    NeuralNetwork& nn; 
    Environment& env; 

    Agent(Genome &g , NeuralNetwork &n , Environment &e )
    : genome(g), nn(n), env(e) {}


}; 