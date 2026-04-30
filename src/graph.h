#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <map>

struct Edge {
    int to;
    double distance;
    double time;
};

struct PathResult {
    std::vector<int> nodes;
    double totalDistance;
    double totalTime;
    bool found;
};

enum class RouteMode {
    SHORTEST_DISTANCE,
    FASTEST_TIME
};

class Graph;

PathResult findPath(const Graph& graph, int start, int end, RouteMode mode);

class Graph {
public:
    void addEdge(int from, int to, double distance, double time);
    bool loadFromFile(const std::string& filename);
    const std::vector<Edge>& getNeighbors(int node) const;
    bool hasNode(int node) const;
    std::vector<int> getAllNodes() const;

private:
    std::map<int, std::vector<Edge>> adj;
};

#endif
