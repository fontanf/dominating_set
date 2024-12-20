#include "data_types/graph.hpp"
using namespace std;


Graph::Graph(int vertices) {
        numVertices = vertices;
        NumEdges = 0;
        adjList.resize(vertices, {}); // graph with no edge
};

Graph::Graph(const Graph&  G) {
    numVertices = G.getNumVertices();
    adjList.resize(numVertices, {});
    for(int i = 0; i < numVertices; i++){
        for(int v : G.getNeighbours(i)){
            adjList[i].push_back(v);
        }
    }
};


Graph::Graph(std::string filename){
    // Open filename
    ifstream file;
    file.open(filename);
    // Check if the file is successfully opened
    if (!file.is_open()) {
        cerr << "Error opening the file! " << filename << endl;
        exit ( EXIT_FAILURE );
    }

    vector<vector<int>> edges = {}; // the set of all edges (needed to find the number of edges which is not given in the instance files for some reasons)
    int size = -1;
    string line;
    // Read a new line until EOF
    while (getline(file, line)) {
        //std::cout<<"oi"<<std::endl;
        // Split the line 
        std::string node1 = "";
        std::string node2 = "";
        int cpt = 0;

        while(line[cpt] != ' '){
            node1 = node1 + line[cpt];
            cpt++;
        }
        cpt++;
        while(cpt < line.size()){
            node2 = node2 + line[cpt];
            cpt++;
        }
        int n1 = stoi(node1);
        int n2 = stoi(node2);
        if(max(n1, n2) > size){
            size = max(n1, n2);
        } 
        edges.push_back({n1, n2});
    }
    // Close the file
    file.close();
    // We now construct the graph
    size = size + 1;
    numVertices = size;
    adjList.resize(size, {});
    for(int i = 0; i < edges.size(); i++){
        this->addEdge(edges[i][0], edges[i][1]); // Note: names int the instance files start from 1, but our indices start from 0
    } 
};


void Graph::addEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            if(v != u){
                adjList[u].push_back(v);
                adjList[v].push_back(u);
            }
        } else {
            std::cout << "Invalid vertex!" << std::endl; //Error
            exit ( EXIT_FAILURE );
        }
    };


int findPosElement(int e, vector<int> v){ // return the first position of an element in a vector
    for(int i = 0; i < v.size(); i++){
        if(e == v[i])
            return i;
    }
    return -1;
}

void Graph::disconnectVertex(int u){  // Delete all the edges aroud u
    for(int v : this->getNeighbours(u)){
        int pos = findPosElement(u, this->getNeighbours(v)); // locate u in the adj list of v
        if (pos >= 0){
          std::swap(adjList[v][pos], adjList[v][adjList[v].size() - 1]);
          adjList[v].pop_back();
        }
    } 
    adjList[u] = {};
}

bool Graph::hasEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            if(not(findPosElement(u,adjList[v])))
            {
                return false;
            }
            else {
                return true;
            }
        }
        return false;
    };

void Graph::printGraph_console() {
        std::cout << "Adjacency list:" << std::endl;
        for (int i = 0; i < numVertices; ++i){
            std::cout << i << " :";
            for(int e : adjList[i]){
                std::cout << " " << e;
            }
            std::cout << std::endl;
        }
    };

int Graph::vertexCount() {
        return numVertices;
    };



int Graph::getNumVertices() const{
    return this->numVertices;
};


std::vector<int> Graph::getNeighbours(int v) const{
    return this->adjList[v];
};


bool Graph::isIsolated(int v){
    return this->adjList[v].size() == 0;
}

int Graph::getDegree(int v){
    return this->adjList[v].size();
}


