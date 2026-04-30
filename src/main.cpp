#include <iostream>
#include <string>
#include "graph.h"

int main() {
    Graph cityGraph;
    std::string mapFile = "assets/city_map.txt";

    std::cout << "--- Adaptive City Routing ---" << std::endl;
    
    if (!cityGraph.loadFromFile(mapFile)) {
        std::cerr << "Failed to load map data." << std::endl;
        return 1;
    }

    std::cout << "Map loaded successfully." << std::endl;

    int start, end, modeChoice;
    while (true) {
        std::cout << "\nEnter Start Node (or -1 to quit): ";
        if (!(std::cin >> start) || start == -1) break;

        std::cout << "Enter End Node: ";
        std::cin >> end;

        std::cout << "Select Mode (0: Shortest Distance, 1: Fastest Time): ";
        std::cin >> modeChoice;

        RouteMode mode = (modeChoice == 1) ? RouteMode::FASTEST_TIME : RouteMode::SHORTEST_DISTANCE;

        PathResult result = findPath(cityGraph, start, end, mode);

        if (result.found) {
            std::cout << "Path found: ";
            for (size_t i = 0; i < result.nodes.size(); ++i) {
                std::cout << result.nodes[i] << (i == result.nodes.size() - 1 ? "" : " -> ");
            }
            std::cout << "\nTotal Distance: " << result.totalDistance << " units";
            std::cout << "\nTotal Time: " << result.totalTime << " mins" << std::endl;
        } else {
            std::cout << "No path exists between " << start << " and " << end << "." << std::endl;
        }
    }

    std::cout << "Exiting. Goodbye!" << std::endl;
    return 0;
}
