#pragma once 
#include "Genome.hpp"
#include <Eigen/Dense>
#include <vector> 
#include <stack>
#include <unordered_set> 
#include <iostream> 

// this is the phenotype that is based off the inputted genome 
//CURRENTLY IS BASED ON A TOPLOGICAL/GRAPHICAL REPRESENTATION.  
// TODO: optimize into a more linear algebra friendly framework 
class NeuralNetwork{
    std::vector<int> evalOrder; // order nodes should be calculated in 
    std::unordered_map<int, NodeType> nodeTypes; // for quick type checking 
    std::unordered_map<int, std::vector<Connection>> incomingConnections; // to keep track of what nodes to pull input from when calculating this nodes output 

public:
    NeuralNetwork(Genome genome){ 
        
        //TODO: the initial stack has hidden in it to should only have inputs in there 
        std::stack<int> nodeStack; // for traversal 
        std::cout<<"Initial Stack Contents: "<<std::endl;
        for(Node node : genome.nodes){
            nodeTypes[node.id] = node.type; 
            if(node.type == NodeType::INPUT || node.type == NodeType::BIAS)
                nodeStack.push(node.id); //start traversing at the inputs
                std::cout<<node.id<<" "; 
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
        std::unordered_set<int> visited; 
        std::cout<<"Starting Topological Sort: "<<std::endl; 
        while (!nodeStack.empty()){
            int currNode = nodeStack.top();
            
            // if doesn't have any non-visited neighbors then visit, add to eval order, and pop
            if (adjList[currNode].empty()){
                visited.insert(currNode); 
                evalOrder.insert(evalOrder.begin(), currNode); // add to front
                nodeStack.pop(); 
                std::cout<<currNode<<std::endl; 
            }else{ // add neighbors to the stack 
                for (int node : adjList[currNode]){
                    // if the current node is in visited set, skip
                    auto it = visited.find(node); 
                    
                    if(it == visited.end())
                        nodeStack.push(node); 
                }
            }
        }
        // eval order 
        for (int node : evalOrder){
            std::cout<< node <<" "; 
        }
    }

    Eigen::VectorXd feedForward(Eigen::VectorXd input ){ // get output based on input, input dim should match up 
        
    }; 
}; 