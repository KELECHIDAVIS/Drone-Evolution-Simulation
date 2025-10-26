#pragma once
#include <vector> 
#include <json.hpp>
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
    Node() : id(-1), type(NodeType::HIDDEN) {}
    Node(int i , NodeType t): 
    id(i), type(t){}

    nlohmann::json to_json() const {
        return nlohmann::json{
            {"id", id},
            {"type", static_cast<int>(type)}
        };
    }

}; 

//CONNECTIONS THAT ALR EXIST WITH A CERTAIN INNV NUM WILL HAVE THAT SAME INNOV NUM 
// for ex: if 2->3 is mutated but that connection was alr found with an innov num of 4, set the same innov num to the mutation
struct Connection{ // connects two nodes with a weight.
    int inId ;  
    int outId; 
    double weight; 
    bool isEnabled;
    bool isRecurrent; 
    int innvNum; // innovation number; the historical marker for when this connection appeared   
    Connection() : inId(-1), outId(-1), weight(0.0), isEnabled(true), innvNum(-1), isRecurrent(false) {}
    Connection(int in, int out, double w, bool en, int innov,bool rec)
        : inId(in), outId(out), weight(w), isEnabled(en),  innvNum(innov),isRecurrent(rec) {}

    nlohmann::json to_json() const {
        return nlohmann::json {
            {"inId", inId},
            {"outId", outId},
            {"weight", weight},
            {"isEnabled", isEnabled},
            {"innvNum", innvNum},
            {"isRecurrent", isRecurrent}
        };
    }
}; 
struct Genome{ // the blueprint to build the neural network 
    std::vector<Node> nodes;    
    std::vector<Connection> connections; 
    
    int speciesID= -1; // unset species  
    double fitness=0.0; 
    double adjustedFitness=0.0; // fitness divided by number of organisms in the same species
    // Add a node (local ID unique in this genome)
    void addNode(NodeType type) {
        int newId = nodes.empty() ? 0 : (nodes.back().id + 1);
        nodes.emplace_back(newId, type);
    }
    void addNode(NodeType type, int id ) {
        nodes.emplace_back(id, type);
    }
    // Add a connection with a new global innovation number
    void addConnection(int in, int out, double weight, bool enabled, int innovation, bool isRecurrent) {
        connections.emplace_back(in, out, weight, enabled, innovation, isRecurrent);
    }

    nlohmann::json to_json() const {
        nlohmann::json nodes_json = nlohmann::json::array();
        for (const auto& node : nodes) {
            nodes_json.push_back(node.to_json());
        }
        
        nlohmann::json conns_json = nlohmann::json::array();
        for (const auto& conn : connections) {
            conns_json.push_back(conn.to_json());
        }
        
        return nlohmann::json{
            {"fitness", fitness},
            {"adjustedFitness", adjustedFitness},
            {"nodes", nodes_json},
            {"connections", conns_json}
        };
    }
}; 

struct Species{ 
    int id; 
    double bestFitness ; //  best raw fitness 
    double sumOfAdjFits; //to assign number of offspring produced by a certain species 
    int appearedInGen; // what generation did it appear 
    int gensSinceImprovement; // to determine stagnant species 
    std::vector<Genome> members; 
}; 