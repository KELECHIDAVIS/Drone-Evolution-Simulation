#include <vector> 
// genomes are the blueprint for creating a network 
enum NodeType{
    INPUT,
    OUTPUT,
    HIDDEN
}; 

struct Node{
    int id ; 
    NodeType type; 
    Node(int id, NodeType type ): id(id), type(type) {}
}; 

struct Connection{
    int inId; // input id 
    int outId; //output id 
    double weight; 
    bool enabled; 
    int innovId; // innovation num 
    Connection(int in , int out , double w, bool en , int innv ): inId(in), outId(out), weight(w), enabled(en), innovId(innv) {}
}; 

struct Genome{
    std::vector<Node> nodes ; 
    std::vector<Connection> connections; 
    double fitness = 0.0; // set after evaluation ; higher is better 

    void addNode(int id, NodeType type){
        nodes.emplace_back(Node(id , type)); 
    }
    void addConnection(int in , int out , double w, bool en , int innv  ){
        connections.emplace_back(Connection(in , out , w , en , innv)); 
    }
}; 


