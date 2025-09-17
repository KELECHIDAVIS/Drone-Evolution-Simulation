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
    
    void dfs(int currNode, std::unordered_set<int> &visited, std::unordered_map<int, 
        std::vector<int>> &adjList, std::vector<int> &evalOrder){
        
        // if no children (output node ), mark visited and add to eval 
        if(adjList.find(currNode) == adjList.end() )
            adjList[currNode] ; // make empty list for no children 
            
        for (int child : adjList[currNode]){
            if (visited.find(child) == visited.end()) // only add unvisited children 
                dfs(child, visited, adjList, evalOrder); 
        }
        visited.insert(currNode); 
        evalOrder.push_back(currNode); 
        
    }
    NeuralNetwork(Genome genome){ 
        
        
        for(Node node : genome.nodes){
            nodeTypes[node.id] = node.type; 
        }
        
        
        std::unordered_map<int , std::vector<int>> adjList; // since dag, the adjList will list out the outgoing connections 
        
        for(Connection conn : genome.connections){ // get each nodes incoming connections as well as just building and adj list 
            if (conn.isEnabled){
                incomingConnections[conn.outId].push_back(conn);
                adjList[conn.inId].push_back(conn.outId); 
            }   
        }

        // through topological sort, get the order of node evaluation of this genome 
        std::unordered_set<int> visited; 
        for ( Node node : genome.nodes){
            if(visited.find(node.id) == visited.end())
                dfs(node.id, visited, adjList, evalOrder); 
        }

        // reverse the eval order 
        std::reverse(evalOrder.begin(), evalOrder.end()); 
        
    }

    
    Eigen::VectorXd feedForward(Eigen::VectorXd input ){ // get output based on input, input dim should match up 
        // iterate through eval order and store the value of the calc based on the type of node
        std::unordered_map<int, double> values; 
        // set input vals 
        for( int nodeID : evalOrder)    {
            if(nodeTypes[nodeID] == NodeType::INPUT){
                values[nodeID] = input(nodeID);  // because input nodes r first created can set as index as well
            }else if (nodeTypes[nodeID] == NodeType::INPUT){
                values[nodeID] = 1.0; 
            }else{ // output or hidden that will be computed later 
                values[nodeID] = 0.0; 
            }
        }

        // now iterate through eval order again using incoming connections to get the weight sum at each node 
        for (int nodeID : evalOrder){
            if (nodeTypes[nodeID] == NodeType::INPUT || nodeTypes[nodeID] == NodeType::BIAS)   
                continue; // skip if input 

            double sum = 0; 
            // for each incoming connection, add the weighted sum 
            for (Connection conn : incomingConnections[nodeID]){
                sum+= conn.weight * values[conn.inId]; 
            }

            // apply activation and set value of node 
            sum = steepenedSigmoid(sum); 

            values[nodeID] =sum ; 
        }
    }

    double steepenedSigmoid(double x){
        return 1.0 / (1 + exp(-4.9*x)); 
    }

}; 