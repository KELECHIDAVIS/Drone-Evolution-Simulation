#pragma once 
#include "Genome.hpp"
#include <Eigen/Dense>
#include <vector> 
#include <stack>
#include <set> 


// this is the phenotype that is based off the inputted genome 
//CURRENTLY IS BASED ON A TOPLOGICAL/GRAPHICAL REPRESENTATION.  
// TODO: optimize into a more linear algebra friendly framework 
class NeuralNetwork{
    std::vector<int> evalOrder; // order nodes should be calculated in 
    std::unordered_map<int, NodeType> nodeTypes; // for quick type checking 
    std::unordered_map<int, std::vector<Connection>> incomingConnections; // to keep track of what nodes to pull input from when calculating this nodes output 

public:
    NeuralNetwork(Genome genome){ 
        
        std::stack<int> nodeStack; // for traversal 
        for(Node node : genome.nodes){
            nodeTypes[node.id] = node.type; 
            if(node.type == NodeType::INPUT || node.type == NodeType::BIAS)
                nodeStack.push(node.id); //start traversing at the inputs
        }

        std::unordered_map<int , std::vector<int>> adjList; // since dag, the adjList will list out the outgoing connections 
        
        for(Connection conn : genome.connections){ // get each nodes incoming connections as well as just building and adj list 
            if (conn.isEnabled){
                incomingConnections[conn.outId].push_back(conn);
                adjList[conn.inId].push_back(conn.outId); 
            }   
        }

        // through topological sort, get the order of the evaluation of this genome 
        // add all inputs to stack, loop will finish when it is empty
        // std::set<int> visited; 
        // while (!nodeStack.empty()){
        //     int currNode = nodeStack.top(); 
        //     visited.insert(currNode); // current node is visited 
        //     // get curr node's neighbors, if no neighbors add to evalOrder
        //     if(adjList[currNode].size() ==0){

        //     }
        // }

    }

    Eigen::VectorXd feedForward(Eigen::VectorXd input ){ // get output based on input, input dim should match up 

    }; 
}; 