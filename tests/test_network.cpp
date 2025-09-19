#include <iostream>
#include "NeuralNetwork.hpp"  // adjust include path

int main() {
    // Simple dummy check
    NeuralNetwork net; 
    if (net.numOutputs() == 0) {
        std::cout << "PASS\n";
        return 0;
    } else {
        std::cerr << "FAIL\n";
        return 1;
    }
}
