#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>

enum class RouteMode {
    SHORTEST_DISTANCE,
    FASTEST_TIME,
    CHEAPEST_ROUTE,
    CUSTOM_WEIGHTED
};

struct Edge {
    int to;
    double distance;
    double time;
    double cost;
    std::string type;
};

struct RelaxationStep {
    int visitedNode;
    std::map<int, double> currentDistances;
    std::map<int, int> predecessors;
};

struct PathResult {
    std::vector<int> nodes;
    std::vector<RelaxationStep> relaxationTable;
    double totalDistance = 0;
    double totalTime = 0;
    double totalCost = 0;
    double executionTimeMs = 0;
    bool found = false;
};

struct CustomWeights {
    float distanceWeight = 1.0f;
    float timeWeight = 0.0f;
    float costWeight = 0.0f;
};

struct NodePos {
    float x, y;
};

class Graph {
public:
    void addEdge(int from, int to, double distance, double time, double cost, std::string type = "road");
    void updateEdge(int from, int to, double distance, double time, double cost, std::string type = "");
    void removeEdge(int from, int to);
    void removeNode(int id);
    bool loadFromFile(const std::string& filename);
    bool loadFromData(const std::string& data); // New: Load from embedded data
    bool saveToFile(const std::string& filename); // New: Save current map
    const std::vector<Edge>& getNeighbors(int node) const;
    bool hasNode(int node) const;
    std::vector<int> getAllNodes() const;
    
    void addNode(int id); 
    std::map<int, NodePos>& getPositions() { return nodePositions; }
    std::map<int, std::vector<Edge>>& getAdjList() { return adj; }
    void setNodePos(int node, float x, float y) { addNode(node); nodePositions[node] = {x, y}; }
    void applyCircleLayout(float centerX, float centerY, float radius);

    static std::string idToLabel(int id) {
        if (id <= 0) return std::to_string(id);
        std::string label = "";
        while (id > 0) {
            id--;
            label = (char)('A' + (id % 26)) + label;
            id /= 26;
        }
        return label;
    }

    static int labelToId(std::string label) {
        int id = 0;
        for (char c : label) {
            char uc = (char)std::toupper((unsigned char)c); // Auto-uppercase
            if (uc < 'A' || uc > 'Z') continue;
            id = id * 26 + (uc - 'A' + 1);
        }
        return id;
    }

private:
    std::map<int, std::vector<Edge>> adj;
    std::map<int, NodePos> nodePositions;
    std::set<int> nodeIds;
};

PathResult findPath(const Graph& graph, int start, int end, RouteMode mode, 
                    const CustomWeights& weights = {}, 
                    const std::set<int>& avoidedNodes = {},
                    const std::set<std::string>& avoidedTypes = {});

#endif
