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
    
    void dfs(int currNode, std::unordered_set<int> &visited, std::unordered_map<int, std::vector<int>> &adjList, std::vector<int> &evalOrder){
        
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

        //TODO: remove print adj list (testing)
        std::cout<<"\nPrinting adj list: "<<std::endl; 
        for (auto node : adjList ){
            std::cout<<node.first<< ": [ "; 
            for (int neighbor: node.second){
                std::cout<<neighbor<<" "; 
            }
            std::cout<<"]"<<std::endl; 
        }

        // through topological sort, get the order of node evaluation of this genome 
        std::unordered_set<int> visited; 
        std::cout<<"\nTopological Sort Result: "<<std::endl; 
        for ( Node node : genome.nodes){
            if(visited.find(node.id) == visited.end())
                dfs(node.id, visited, adjList, evalOrder); 
        }

        // reverse the eval order 
        std::reverse(evalOrder.begin(), evalOrder.end()); 
        // eval order 
        for (int node : evalOrder){
            std::cout<< node <<" "; 
        }
    }

    Eigen::VectorXd feedForward(Eigen::VectorXd input ){ // get output based on input, input dim should match up 
        
    }; 
}; 