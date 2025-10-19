
#include "NEATRunner.hpp"   

int main() {
    
    NEATRunner runner; 

    #ifdef NDEBUG
        // Release build
        int generations = 2500; // release can handles much more generations much faster (1000+)
    #else
        // Debug build
        int generations = 100;
    #endif

    for(int i =0; i<generations; i++){
        runner.runGeneration(); 
    }
    return 0 ; 
}