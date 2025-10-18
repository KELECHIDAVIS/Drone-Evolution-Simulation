
#include "NEATRunner.hpp"   

int main() {
    
    NEATRunner runner; 
    
    for(int i =0; i<100; i++){
        runner.runGeneration(); 
    }
    return 0 ; 
}