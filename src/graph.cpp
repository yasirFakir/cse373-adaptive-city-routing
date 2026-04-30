#include "graph.h"

void Graph::addEdge(int from, int to, double weight, double time) {
    adj[from].push_back({to, weight, time});
}
