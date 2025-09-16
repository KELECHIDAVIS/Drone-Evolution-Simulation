#pragma once
#include <vector> 
// genomes include node genes and connection genes that later determine the phenotype of the agent (neural network)

enum NodeType{
    INPUT,
    OUTPUT, 
    HIDDEN,
    BIAS // optional
}; 
struct Node{ // at each node that isn't an input or bias, their will be an activation of the resulting value 
    int id; // starts at 0
    NodeType type; 
}; 

//CONNECTIONS THAT ALR EXIST WITH A CERTAIN INNV NUM WILL HAVE THAT SAME INNOV NUM 
// for ex: if 2->3 is mutated but that connection was alr found with an innov num of 4, set the same innov num to the mutation
struct Connection{ // connects two nodes with a weight.
    int inId ;  
    int outId; 
    double weight; 
    bool isEnabled;
    int innvNum; // innovation number; the historical marker for when this connection appeared   
}; 
struct Genome{ // the blueprint to build the neural network 
    std::vector<Node&> nodes;    
    std::vector<Connection&> connections; 

    void addNode(Node &node){ // through evolution new hidden nodes and connections may be added at back of list 
        nodes.push_back(node);  
    }
    void addConnection(Connection &conn){ 
        connections.push_back(conn); 
    }
}; 