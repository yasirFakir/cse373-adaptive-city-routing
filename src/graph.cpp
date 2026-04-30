#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>

void Graph::addEdge(int from, int to, double distance, double time) {
    adj[from].push_back({to, distance, time});
    // Assuming directed graph based on file format, 
    // but usually city maps are undirected. 
    // Adding reverse edge for a typical city map.
    adj[to].push_back({from, distance, time});
}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        int from, to;
        double dist, time;
        if (ss >> from >> to >> dist >> time) {
            addEdge(from, to, dist, time);
        }
    }
    return true;
}

const std::vector<Edge>& Graph::getNeighbors(int node) const {
    static const std::vector<Edge> empty;
    auto it = adj.find(node);
    return (it != adj.end()) ? it->second : empty;
}

bool Graph::hasNode(int node) const {
    return adj.find(node) != adj.end();
}

std::vector<int> Graph::getAllNodes() const {
    std::vector<int> nodes;
    for (auto const& [node, _] : adj) {
        nodes.push_back(node);
    }
    return nodes;
}
