#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <map>

struct Edge {
    int to;
    double weight;
    double time;
};

class Graph {
public:
    void addEdge(int from, int to, double weight, double time);
    // Add more methods as needed
private:
    std::map<int, std::vector<Edge>> adj;
};

#endif
