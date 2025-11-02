
#include "NEATRunner.hpp"   
using namespace std; 
int main() {
    
    NEATRunner runner; 
    
    #ifdef NDEBUG
        // Release build
        int generations = 300; // release can handles much more generations much faster (1000+)
    #else
        // Debug build
        int generations = 10;
    #endif

    for(int i =0; i<generations; i++){
        runner.runGeneration(); 
    }

    int returnCode = system("py ../analysis/show_training_results.py"); 

    if (returnCode == 0)
    {
        cout << "Python Visualizer Launched Successfully" << endl;
    }
    else
    {
        cout << "Python Visualizer failed to launch; "
                "error code: "
             << returnCode << endl;
    }
    return 0 ; 
}