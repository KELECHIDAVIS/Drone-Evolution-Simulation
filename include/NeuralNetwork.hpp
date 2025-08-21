#pragma once 
#include "Genome.hpp"
#include <Eigen/Sparse> // sparse matrices since most connections would be 0 

struct NeuralNetwork{
    std::vector<Eigen::MatrixXd> weights; 
    std::vector<Eigen::VectorXd> biases; // optional 
    std::vector<int> layerSizes;  
}; 


// build the network from genome 
NeuralNetwork buildNetwork(Genome &genome){
    // compute depth for each node ; (nodeId, depth)
    std::unordered_map<int, int > depth; 
    
    for(auto &node : genome.nodes){
        if(node.type == NodeType::INPUT || node.type == NodeType::BIAS)
            depth[node.id] = 0; 
    }

    // iteratively go through connections until nothing changes 
    bool changed = true; 
    while(changed){
        changed =false; 
        for(auto &conn : genome.connections){
            // figure out what input/bias it's connected to ; depth = maxDepth +1 
            if(!conn.enabled) continue; 
            
            int in= conn.inId, out = conn.outId; 
            if (depth.count(in)) {
                int newDepth = depth[in] + 1;
                if (!depth.count(out) || newDepth > depth[out]) {
                    depth[out] = newDepth;
                    changed = true;
                }
            }
        }
    }

    // group by depth 
    int maxDepth = 0;
    for (auto& kv : depth) maxDepth = std::max(maxDepth, kv.second);

    //  each layer holds the node ids within that layer 
    std::vector<std::vector<int>> layers(maxDepth+1);
    for (auto& kv : depth) {
        layers[kv.second].push_back(kv.first);
    }

    // build out weight matrices layer by layer 
    NeuralNetwork net;
    for (int l = 1; l <= maxDepth; l++) {
        int inSize = layers[l-1].size();
        int outSize = layers[l].size();
        Eigen::MatrixXd W = Eigen::MatrixXd::Zero(outSize, inSize);

        // Fill matrix from connections
        for (int i = 0; i < outSize; i++) {
            int outNode = layers[l][i];
            for (int j = 0; j < inSize; j++) {
                int inNode = layers[l-1][j];
                for (auto& conn : genome.connections) {
                    if (conn.enabled && conn.inId == inNode && conn.outId == outNode) {
                        W(i,j) = conn.weight;
                    }
                }
            }
        }
        net.weights.push_back(W);
        net.biases.push_back(Eigen::VectorXd::Zero(outSize)); // optional biases
        net.layerSizes.push_back(outSize);
    }
    return net; 
}; 

//this is for general use on the main side multiply by the actual ranges 
Eigen::VectorXd forward(const NeuralNetwork& net, const Eigen::VectorXd& input) {
    Eigen::VectorXd a = input;

    for (size_t l = 0; l < net.weights.size(); l++) {
        Eigen::VectorXd z = net.weights[l] * a + net.biases[l];
        // Apply activation elementwise (sigmoid here)
        for (int i = 0; i < z.size(); i++) {
            z(i) = 1.0 / (1.0 + std::exp(-z(i)));
        }
        a = z;
    }
    return a;
}