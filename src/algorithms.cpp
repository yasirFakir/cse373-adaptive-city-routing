#include "graph.h"
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>

PathResult findPath(const Graph& graph, int start, int end, RouteMode mode) {
    std::map<int, double> minCost;
    std::map<int, int> parent;
    std::map<int, double> distAtNode;
    std::map<int, double> timeAtNode;

    for (int node : graph.getAllNodes()) {
        minCost[node] = std::numeric_limits<double>::infinity();
    }

    using NodeCost = std::pair<double, int>;
    std::priority_queue<NodeCost, std::vector<NodeCost>, std::greater<NodeCost>> pq;

    minCost[start] = 0;
    distAtNode[start] = 0;
    timeAtNode[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        double d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > minCost[u]) continue;
        if (u == end) break;

        for (const auto& edge : graph.getNeighbors(u)) {
            double weight = (mode == RouteMode::SHORTEST_DISTANCE) ? edge.distance : edge.time;
            if (minCost[u] + weight < minCost[edge.to]) {
                minCost[edge.to] = minCost[u] + weight;
                parent[edge.to] = u;
                distAtNode[edge.to] = distAtNode[u] + edge.distance;
                timeAtNode[edge.to] = timeAtNode[u] + edge.time;
                pq.push({minCost[edge.to], edge.to});
            }
        }
    }

    PathResult result;
    if (minCost.find(end) == minCost.end() || minCost[end] == std::numeric_limits<double>::infinity()) {
        result.found = false;
        return result;
    }

    result.found = true;
    result.totalDistance = distAtNode[end];
    result.totalTime = timeAtNode[end];

    for (int v = end; v != start; v = parent[v]) {
        result.nodes.push_back(v);
    }
    result.nodes.push_back(start);
    std::reverse(result.nodes.begin(), result.nodes.end());

    return result;
}
