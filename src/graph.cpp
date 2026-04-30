#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cmath>

void Graph::addNode(int id) {
    if (id <= 0) return;
    nodeIds.insert(id);
    if (adj.find(id) == adj.end()) adj[id] = {};
}

void Graph::removeNode(int id) {
    if (nodeIds.count(id) == 0) return;
    for (auto& [u, neighbors] : adj) {
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(), [id](const Edge& e) { return e.to == id; }), neighbors.end());
    }
    adj.erase(id); nodeIds.erase(id); nodePositions.erase(id);
}

void Graph::addEdge(int from, int to, double distance, double time, double cost, std::string type) {
    if (from <= 0 || to <= 0) return;
    addNode(from); addNode(to);
    adj[from].push_back({to, distance, time, cost, type});
    adj[to].push_back({from, distance, time, cost, type});
}

void Graph::applyCircleLayout(float centerX, float centerY, float radius) {
    std::vector<int> nodes = getAllNodes();
    if (nodes.empty()) return;
    float angleStep = 2.0f * 3.14159f / (float)nodes.size();
    for (size_t i = 0; i < nodes.size(); ++i) {
        nodePositions[nodes[i]] = { centerX + radius * cos(i * angleStep), centerY + radius * sin(i * angleStep) };
    }
}

void Graph::updateEdge(int from, int to, double distance, double time, double cost) {
    auto update = [&](int u, int v) {
        for (auto& edge : adj[u]) {
            if (edge.to == v) { edge.distance = distance; edge.time = time; edge.cost = cost; return; }
        }
    };
    update(from, to); update(to, from);
}

void Graph::removeEdge(int from, int to) {
    auto cleanup = [&](int u, int v) {
        auto& neighbors = adj[u];
        neighbors.erase(std::remove_if(neighbors.begin(), neighbors.end(), [v](const Edge& e) { return e.to == v; }), neighbors.end());
    };
    cleanup(from, to); cleanup(to, from);
}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    nodeIds.clear(); adj.clear(); nodePositions.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string uStr, vStr; double dist, time, cost;
        if (ss >> uStr >> vStr >> dist >> time >> cost) {
            auto parseToken = [](std::string s) {
                if (s.empty()) return 0;
                if (std::isdigit(s[0])) return std::stoi(s);
                return labelToId(s);
            };
            addEdge(parseToken(uStr), parseToken(vStr), dist, time, cost, "road");
        }
    }
    return true;
}

bool Graph::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << "# Node1 Node2 Distance Time Cost\n";
    std::set<std::pair<int, int>> seen;
    for (auto const& [u, neighbors] : adj) {
        for (auto const& edge : neighbors) {
            int v = edge.to;
            if (seen.count({v, u})) continue;
            file << idToLabel(u) << " " << idToLabel(v) << " " 
                 << edge.distance << " " << edge.time << " " << edge.cost << "\n";
            seen.insert({u, v});
        }
    }
    return true;
}

const std::vector<Edge>& Graph::getNeighbors(int node) const {
    static const std::vector<Edge> empty;
    auto it = adj.find(node);
    return (it != adj.end()) ? it->second : empty;
}

bool Graph::hasNode(int node) const { return nodeIds.count(node) > 0; }
std::vector<int> Graph::getAllNodes() const { return std::vector<int>(nodeIds.begin(), nodeIds.end()); }
