#include "graph.h"
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>

PathResult findPath(const Graph& graph, int start, int end, RouteMode mode, 
                    const CustomWeights& weights, 
                    const std::set<int>& avoidedNodes,
                    const std::set<std::string>& avoidedTypes) {
    auto startTime = std::chrono::high_resolution_clock::now();
    PathResult result;
    result.found = false;

    if (!graph.hasNode(start) || !graph.hasNode(end)) return result;

    std::map<int, double> dist;
    std::map<int, int> parent;
    std::map<int, double> distAtNode, timeAtNode, costAtNode;
    std::vector<int> allNodes = graph.getAllNodes();

    for (int node : allNodes) {
        dist[node] = std::numeric_limits<double>::infinity();
        parent[node] = -1;
    }

    dist[start] = 0;
    distAtNode[start] = timeAtNode[start] = costAtNode[start] = 0;

    // Step 0 initialization in the table
    result.relaxationTable.push_back({ -1, dist, parent });

    using NodeCost = std::pair<double, int>;
    std::priority_queue<NodeCost, std::vector<NodeCost>, std::greater<NodeCost>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;

        // Process neighbors (Relaxation)
        for (const auto& edge : graph.getNeighbors(u)) {
            if (avoidedNodes.count(edge.to) || avoidedTypes.count(edge.type)) continue;

            double weight = 0;
            switch(mode) {
                case RouteMode::SHORTEST_DISTANCE: weight = edge.distance; break;
                case RouteMode::FASTEST_TIME:      weight = edge.time; break;
                case RouteMode::CHEAPEST_ROUTE:    weight = edge.cost; break;
                case RouteMode::CUSTOM_WEIGHTED:
                    weight = (edge.distance * weights.distanceWeight) + (edge.time * weights.timeWeight) + (edge.cost * weights.costWeight);
                    break;
            }

            if (dist[u] + weight < dist[edge.to]) {
                dist[edge.to] = dist[u] + weight;
                parent[edge.to] = u;
                distAtNode[edge.to] = distAtNode[u] + edge.distance;
                timeAtNode[edge.to] = timeAtNode[u] + edge.time;
                costAtNode[edge.to] = costAtNode[u] + edge.cost;
                pq.push({dist[edge.to], edge.to});
            }
        }
        
        // Record step after relaxing all neighbors of u
        result.relaxationTable.push_back({ u, dist, parent });
        if (u == end) break;
    }

    if (dist.count(end) && dist[end] != std::numeric_limits<double>::infinity()) {
        result.found = true;
        result.totalDistance = distAtNode[end];
        result.totalTime = timeAtNode[end];
        result.totalCost = costAtNode[end];
        for (int v = end; v != -1; v = parent[v]) {
            result.nodes.push_back(v);
            if (v == start) break;
        }
        std::reverse(result.nodes.begin(), result.nodes.end());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    return result;
}
