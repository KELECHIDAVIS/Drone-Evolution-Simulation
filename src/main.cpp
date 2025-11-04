
#include "NEATRunner.hpp"   
using namespace std; 
int main() {
    
    NEATRunner runner; 
    
    

    for(int i =0; i<NEATRunner::GENS ; i++){
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