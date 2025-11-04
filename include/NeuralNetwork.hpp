#pragma once 
#include "Genome.hpp"
#include <Eigen/Dense>
#include <vector> 
#include <stack>
#include <unordered_set> 
#include <iostream> 


// Diff activations for testing 
    
enum ActivationType { SIGMOID, TANH };

// this is the phenotype that is based off the inputted genome 
//CURRENTLY IS BASED ON A TOPLOGICAL/GRAPHICAL REPRESENTATION.  

class NeuralNetwork{
    std::vector<int> evalOrder; // order nodes should be calculated in 
    std::unordered_map<int, NodeType> nodeTypes; // for quick type checking 
    std::unordered_map<int, std::vector<Connection>> incomingConnections; // to keep track of what nodes to pull input from when calculating this nodes output 
    std::vector<int> outputNodeIds;
    std::unordered_map<int, double> previousActivations; // to allow for recurrent connections
    bool isFirstStep; // track if first forward pass occured
    
    
public:
    
    ActivationType activationType = TANH;
    
    void dfs(int currNode, std::unordered_set<int> &visited, std::unordered_map<int, 
        std::vector<int>> &adjList, std::vector<int> &evalOrder){
        
        using namespace std; 
        cout<<"Visited: "; 
        for (int nodes : visited ){
            cout<<nodes<<" "; 
        }cout<<"\n"; 

        cout<<"adjList Contents [\n "; 
        for (auto& pair : adjList ){
            cout<<pair.first<<": "; 
            for(int neighbor : pair.second){
                cout<<neighbor <<" ";
            }cout<<"\n"; 
        }cout<<"]\n";

        cout<<"Eval order: "; 
        for (int nodes : evalOrder ){
            cout<<nodes<<" "; 
        }cout<<"\n"; 
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
    NeuralNetwork(){}; 
    NeuralNetwork(Genome genome): NeuralNetwork(genome, SIGMOID){} // sigmoid is default  
    NeuralNetwork(Genome genome, ActivationType type ){ 
        isFirstStep=true; 
        activationType = type; 
        for(Node node : genome.nodes){
            nodeTypes[node.id] = node.type; 
            if (node.type == NodeType::OUTPUT) {
                outputNodeIds.push_back(node.id);
            }
            previousActivations[node.id] = 0.0 ;
        }
        
        
        std::unordered_map<int , std::vector<int>> adjList; // since dag, the adjList will list out the outgoing connections 
        
        for(Connection conn : genome.connections){ // get each nodes incoming connections as well as just building and adj list 
            if (conn.isEnabled){
                incomingConnections[conn.outId].push_back(conn);

                // ONLY ADD NON RECURRENT CONNECTIONS TO ADJ LIST
                if(!conn.isRecurrent){
                    adjList[conn.inId].push_back(conn.outId); 
                }
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
        
        // Make sure outputs are in ascending order of ID (if that matches your convention)
        std::sort(outputNodeIds.begin(), outputNodeIds.end());
        
    }

    
    Eigen::VectorXd feedForward(Eigen::VectorXd input ){ // get output based on input, input dim should match up 
        // iterate through eval order and store the value of the calc based on the type of node
        std::unordered_map<int, double> values; 
        // set input vals 
        for( int nodeID : evalOrder)    {
            if(nodeTypes[nodeID] == NodeType::INPUT){
                values[nodeID] = input(nodeID);  // because input nodes r first created can set as index as well
            }else if (nodeTypes[nodeID] == NodeType::BIAS){
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
                if(conn.isRecurrent) {
                    // For recurrent connections, use previous activation
                    // BUT: if the source is INPUT or BIAS, use current value
                    if (nodeTypes[conn.inId] == NodeType::INPUT || 
                        nodeTypes[conn.inId] == NodeType::BIAS) {
                        sum += conn.weight * values[conn.inId];
                    } else {
                        sum += conn.weight * previousActivations[conn.inId]; 
                    }
                } else {
                    sum += conn.weight * values[conn.inId]; 
                }
            }

            //TODO: experiment to find the best activation 
            sum = activate(sum);  
            values[nodeID] =sum ; 
        }

        // Store current activations for next timestep
        // Only store activations for HIDDEN and OUTPUT nodes
        for (int nodeID : evalOrder) {
            if (nodeTypes[nodeID] != NodeType::INPUT && 
                nodeTypes[nodeID] != NodeType::BIAS) {
                previousActivations[nodeID] = values[nodeID];
            }
        }
        isFirstStep = false;

        // now convert the values output output vector 
        // Build output vector and MAP to (0, 1) for rocket controls
        Eigen::VectorXd output(outputNodeIds.size());
        for (size_t i = 0; i < outputNodeIds.size(); ++i) {
            // Map from (-1, 1) to (0, 1) if tanh 
            if (activationType == TANH){
                output(i) = (values[outputNodeIds[i]] + 1.0) / 2.0;
            }else {
                output(i) = values[outputNodeIds[i]]; // sigmoid is already returns btwn 0 nd 1 
            }
        }
        return output;
    }

    // SHOULD RUN BETWEEN EPISODES SO MEMORY DOESN'T BLEED OVER
    void reset() {
        // Clear previous activations between episodes
        for (auto& pair : previousActivations) {
            pair.second = 0.0;
        }
        isFirstStep = true;
    }

    double activate(double x) {
        if (activationType == TANH) {
            return std::tanh(2.5 * x);
        } else {
            return steepenedSigmoid(x);
        }
    }
    double steepenedSigmoid(double x){
        return 1.0 / (1 + exp(-4.9*x)); 
    }


    double sigmoid(double x){
        return 1.0 / (1 + exp(-x)); 
    }

    double tanh(double x ){
        return std::tanh(2.5 * x);
    }

    int numOutputs(){
        return outputNodeIds.size(); 
    }

}; 