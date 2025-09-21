#include "NEATRunner.hpp"

NEATRunner::NEATRunner()
{
}

Genome NEATRunner::initGenome()
{
    Genome genome ; 
    
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::INPUT); 
    genome.addNode(NodeType::OUTPUT); 
    genome.addNode(NodeType::OUTPUT); 

    // every time a new connection is added make sure that it increments the global innovation number if it hasn't been made b4
    genome.addConnection(0,6,getRandNum(-2,2), true,0); 
    genome.addConnection(0,7,getRandNum(-2,2), true,1); 
    genome.addConnection(1,6,getRandNum(-2,2), true,2); 
    genome.addConnection(1,7,getRandNum(-2,2), true,3); 
    genome.addConnection(2,6,getRandNum(-2,2), true,4); 
    genome.addConnection(2,7,getRandNum(-2,2), true,5); 
    genome.addConnection(3,6,getRandNum(-2,2), true,6); 
    genome.addConnection(3,7,getRandNum(-2,2), true,7); 
    genome.addConnection(4,6,getRandNum(-2,2), true,8); 
    genome.addConnection(4,7,getRandNum(-2,2), true,9); 
    genome.addConnection(5,6,getRandNum(-2,2), true,10); 
    genome.addConnection(5,7,getRandNum(-2,2), false,11); 
    genome.addConnection(5,8,getRandNum(-2,2), true,12); 
    genome.addConnection(8,7,getRandNum(-2,2), true,13); 

    return genome; 
}