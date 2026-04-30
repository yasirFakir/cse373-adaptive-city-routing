#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include "graph.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#ifdef __linux__
#include <cstdio>
#include <memory>
#include <array>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL_Window* window = nullptr;
SDL_GLContext gl_context = nullptr;
Graph cityGraph;

char startLabel[16] = "A";
char endLabel[16] = "C";
PathResult results[3]; 
bool pathFound = false;
bool isDarkMode = false;
int tableMode = 0; 

bool showDist = true, showTime = false, showCost = false;
int selectedNode = -1, draggedNode = -1, draggingEdgeFrom = -1;
struct SelectedEdge { int from, to; double dist, time, cost; } activeEdge = {-1, -1, 0, 0, 0};
struct Selection { int from, to; } selectedEdge = {-1, -1};

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    if (isDarkMode) {
        ImGui::StyleColorsDark();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    } else {
        ImGui::StyleColorsLight();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    }
    style.WindowRounding = 4.0f;
}

std::string FileDialog(bool save) {
#ifdef __linux__
    std::string cmd = save ? "zenity --file-selection --save --title='Save Map' --filename='map.txt' 2>/dev/null" : "zenity --file-selection --title='Load Map' --file-filter='*.txt' 2>/dev/null";
    std::array<char, 128> buffer;
    std::string result = "";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
#endif
    return "";
}

float DistToSegment(ImVec2 p, ImVec2 v, ImVec2 w) {
    float l2 = pow(v.x - w.x, 2) + pow(v.y - w.y, 2);
    if (l2 == 0.0) return sqrt(pow(p.x - v.x, 2) + pow(p.y - v.y, 2));
    float t = std::max(0.0f, std::min(1.0f, ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2));
    ImVec2 projection = { v.x + t * (w.x - v.x), v.y + t * (w.y - v.y) };
    return sqrt(pow(p.x - projection.x, 2) + pow(p.y - projection.y, 2));
}

void DrawGraph(ImDrawList* drawList, ImVec2 offset, ImVec2 size) {
    auto& positions = cityGraph.getPositions();
    auto& adj = cityGraph.getAdjList();
    ImVec2 mousePos = ImGui::GetMousePos();
    bool mouseClicked = ImGui::IsMouseClicked(0);
    bool mouseDoubleClicked = ImGui::IsMouseDoubleClicked(0);

    ImU32 primaryColor = isDarkMode ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255);
    ImU32 edgeColor = isDarkMode ? IM_COL32(200, 200, 200, 150) : IM_COL32(60, 60, 60, 150);

    for (auto const& [u, neighbors] : adj) {
        for (auto const& edge : neighbors) {
            if (u > edge.to) continue;
            ImVec2 p1 = {positions[u].x + offset.x, positions[u].y + offset.y};
            ImVec2 p2 = {positions[edge.to].x + offset.x, positions[edge.to].y + offset.y};
            bool isHighlighted = false;
            if (pathFound && results[tableMode].found) {
                for (size_t i = 0; i < results[tableMode].nodes.size() - 1; ++i) {
                    if ((results[tableMode].nodes[i] == u && results[tableMode].nodes[i+1] == edge.to) || 
                        (results[tableMode].nodes[i] == edge.to && results[tableMode].nodes[i+1] == u)) {
                        isHighlighted = true; break;
                    }
                }
            }
            bool isSelected = (selectedEdge.from == u && selectedEdge.to == edge.to);
            if (DistToSegment(mousePos, p1, p2) < 12.0f) {
                if (mouseDoubleClicked) { activeEdge = {u, edge.to, edge.distance, edge.time, edge.cost}; ImGui::OpenPopup("Edit Edge"); }
                else if (mouseClicked) { selectedEdge = {u, edge.to}; selectedNode = -1; }
            }
            ImU32 col = isHighlighted ? IM_COL32(0,255,0,255) : (isSelected?IM_COL32(255,140,0,255):edgeColor);
            drawList->AddLine(p1, p2, col, isHighlighted||isSelected?5.0f:2.0f);
            
            ImVec2 mid = {(p1.x + p2.x)/2, (p1.y + p2.y)/2};
            std::string wStr = "";
            if (showDist) wStr += "D:" + std::to_string((int)edge.distance) + " ";
            if (showTime) wStr += "T:" + std::to_string((int)edge.time) + " ";
            if (showCost) wStr += "C:" + std::to_string((int)edge.cost);
            if (!wStr.empty()) {
                ImVec2 tSize = ImGui::CalcTextSize(wStr.c_str());
                drawList->AddRectFilled({mid.x-2, mid.y-2}, {mid.x+tSize.x+2, mid.y+tSize.y+2}, isDarkMode ? IM_COL32(0,0,0,180) : IM_COL32(255,255,255,180));
                drawList->AddText(mid, primaryColor, wStr.c_str());
            }
        }
    }

    if (draggingEdgeFrom != -1) {
        ImVec2 p1 = {positions[draggingEdgeFrom].x + offset.x, positions[draggingEdgeFrom].y + offset.y};
        drawList->AddLine(p1, mousePos, primaryColor, 2.0f);
    }

    for (auto& [id, pos] : positions) {
        ImVec2 p = {pos.x + offset.x, pos.y + offset.y};
        float radius = 22.0f;
        bool hovered = sqrt(pow(mousePos.x - p.x, 2) + pow(mousePos.y - p.y, 2)) < radius;
        if (hovered && ImGui::IsMouseDown(0) && draggingEdgeFrom == -1 && draggedNode == -1) draggedNode = id;
        if (hovered && ImGui::IsMouseDown(1) && draggingEdgeFrom == -1) draggingEdgeFrom = id;
        if (draggedNode == id) { pos.x = std::max(radius, std::min(size.x - radius, mousePos.x - offset.x)); pos.y = std::max(radius, std::min(size.y - radius, mousePos.y - offset.y)); }
        if (hovered && mouseClicked) { selectedNode = id; selectedEdge = {-1, -1}; }
        ImU32 circleColor = (selectedNode == id) ? IM_COL32(255, 255, 0, 255) : (isDarkMode ? IM_COL32(40, 40, 50, 255) : IM_COL32(245, 245, 245, 255));
        drawList->AddCircleFilled(p, radius, circleColor);
        drawList->AddCircle(p, radius, primaryColor, 0, 2.0f);
        std::string label = Graph::idToLabel(id);
        ImVec2 tSize = ImGui::CalcTextSize(label.c_str());
        drawList->AddText({p.x - tSize.x/2, p.y - tSize.y/2}, (selectedNode==id)?IM_COL32(0,0,0,255):primaryColor, label.c_str());
    }

    if (!ImGui::IsMouseDown(0)) draggedNode = -1;
    if (!ImGui::IsMouseDown(1)) {
        if (draggingEdgeFrom != -1) {
            for (auto& [id, pos] : positions) {
                ImVec2 p = {pos.x + offset.x, pos.y + offset.y};
                if (sqrt(pow(mousePos.x - p.x, 2) + pow(mousePos.y - p.y, 2)) < 30.0f && id != draggingEdgeFrom) { cityGraph.addEdge(draggingEdgeFrom, id, 10.0, 5.0, 2.0); break; }
            }
        }
        draggingEdgeFrom = -1;
    }

    if (ImGui::BeginPopupModal("Edit Edge", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Edit: %s <-> %s", Graph::idToLabel(activeEdge.from).c_str(), Graph::idToLabel(activeEdge.to).c_str());
        ImGui::InputDouble("Dist", &activeEdge.dist); ImGui::InputDouble("Time", &activeEdge.time); ImGui::InputDouble("Cost", &activeEdge.cost);
        if (ImGui::Button("Update")) { cityGraph.updateEdge(activeEdge.from, activeEdge.to, activeEdge.dist, activeEdge.time, activeEdge.cost); ImGui::CloseCurrentPopup(); }
        ImGui::SameLine(); if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void main_loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) { ImGui_ImplSDL2_ProcessEvent(&event); if (event.type == SDL_QUIT) exit(0); }
    ApplyTheme();
    ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplSDL2_NewFrame(); ImGui::NewFrame();
    
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (selectedNode != -1) { cityGraph.removeNode(selectedNode); selectedNode = -1; pathFound = false; }
        else if (selectedEdge.from != -1) { cityGraph.removeEdge(selectedEdge.from, selectedEdge.to); selectedEdge = {-1, -1}; pathFound = false; }
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Site", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    float canvasHeight = ImGui::GetIO().DisplaySize.y * 0.60f;
    ImGui::BeginChild("GraphArea", ImVec2(0, canvasHeight), true, ImGuiWindowFlags_NoScrollbar);
    DrawGraph(ImGui::GetWindowDrawList(), ImGui::GetCursorScreenPos(), ImVec2(ImGui::GetIO().DisplaySize.x, canvasHeight));
    
    ImGui::SetCursorPos(ImVec2(ImGui::GetIO().DisplaySize.x - 340, 10));
    if (ImGui::Button("IMPORT MAP", ImVec2(160, 30))) {
        std::string p = FileDialog(false);
        if (!p.empty()) { cityGraph.loadFromFile(p); cityGraph.applyCircleLayout(ImGui::GetIO().DisplaySize.x/2, canvasHeight/2, 200.0f); pathFound = false; }
    }
    ImGui::SameLine();
    if (ImGui::Button("STORE MAP", ImVec2(160, 30))) {
        std::string p = FileDialog(true);
        if (!p.empty()) cityGraph.saveToFile(p);
    }
    ImGui::EndChild();
    
    ImGui::Columns(3, "Layout", true);
    ImGui::SetColumnWidth(0, 320.0f);
    {
        ImGui::TextColored(ImVec4(0, 0.7f, 1, 1), "DIJKSTRA ALGORITHM");
        ImGui::Checkbox("Dark Mode", &isDarkMode);
        ImGui::Separator();
        ImGui::InputText("Start Node", startLabel, 16);
        for(int i=0; startLabel[i]; i++) startLabel[i] = (char)toupper(startLabel[i]);
        ImGui::InputText("End Node", endLabel, 16);
        for(int i=0; endLabel[i]; i++) endLabel[i] = (char)toupper(endLabel[i]);

        if (ImGui::Button("COMPUTE ALL", ImVec2(-1, 35))) {
            int sId = Graph::labelToId(startLabel), eId = Graph::labelToId(endLabel);
            if (sId > 0 && eId > 0) { results[0] = findPath(cityGraph, sId, eId, RouteMode::SHORTEST_DISTANCE); results[1] = findPath(cityGraph, sId, eId, RouteMode::FASTEST_TIME); results[2] = findPath(cityGraph, sId, eId, RouteMode::CHEAPEST_ROUTE); pathFound = true; }
        }
        if (selectedNode != -1 && ImGui::Button("DELETE NODE", ImVec2(-1, 30))) { cityGraph.removeNode(selectedNode); selectedNode = -1; pathFound = false; }
        if (selectedEdge.from != -1 && ImGui::Button("DELETE EDGE", ImVec2(-1, 30))) { cityGraph.removeEdge(selectedEdge.from, selectedEdge.to); selectedEdge = {-1, -1}; pathFound = false; }
        if (ImGui::Button("Add New Node", ImVec2(-1, 30))) { int maxId = 0; auto all = cityGraph.getAllNodes(); for (auto id : all) maxId = std::max(maxId, id); cityGraph.setNodePos(maxId + 1, 200+(rand()%400), 100+(rand()%200)); }
    }
    ImGui::NextColumn();
    {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "RESULTS SUMMARY");
        ImGui::Checkbox("D", &showDist); ImGui::SameLine(); ImGui::Checkbox("T", &showTime); ImGui::SameLine(); ImGui::Checkbox("C", &showCost);
        if (pathFound) {
            if (ImGui::BeginTable("Res", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Mode"); ImGui::TableSetupColumn("D"); ImGui::TableSetupColumn("T"); ImGui::TableSetupColumn("C");
                ImGui::TableHeadersRow();
                const char* n[] = {"Short", "Fast", "Cheap"};
                for (int i=0; i<3; ++i) {
                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("%s", n[i]);
                    if (results[i].found) { ImGui::TableSetColumnIndex(1); ImGui::Text("%.1f", results[i].totalDistance); ImGui::TableSetColumnIndex(2); ImGui::Text("%.1f", results[i].totalTime); ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", results[i].totalCost); }
                    else ImGui::Text("N/A");
                }
                ImGui::EndTable();
            }
            for (int i=0; i<3; ++i) if (results[i].found) {
                std::string p = ""; for (size_t n=0; n<results[i].nodes.size(); ++n) p += Graph::idToLabel(results[i].nodes[n]) + (n == results[i].nodes.size()-1 ? "" : "->");
                ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.4f, 1), "%s:", i==0?"Dist":i==1?"Time":"Cost"); ImGui::TextWrapped("%s", p.c_str());
            }
        }
    }
    ImGui::NextColumn();
    {
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "RELAXATION TRACE");
        const char* m[] = {"Distance", "Time", "Cost"}; ImGui::SetNextItemWidth(120); ImGui::Combo("Metric", &tableMode, m, 3);
        if (pathFound && results[tableMode].found) {
            auto all = cityGraph.getAllNodes(); std::sort(all.begin(), all.end());
            float h = (results[tableMode].relaxationTable.size() + 1) * ImGui::GetTextLineHeightWithSpacing() + 10.0f;
            if (ImGui::BeginTable("Relax", (int)all.size() + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX, ImVec2(0, std::min(h, 200.0f)))) {
                ImGui::TableSetupColumn("St"); ImGui::TableSetupColumn("Nd");
                for (int n : all) ImGui::TableSetupColumn(Graph::idToLabel(n).c_str(), ImGuiTableColumnFlags_WidthFixed, 55);
                ImGui::TableHeadersRow();
                for (size_t i = 0; i < results[tableMode].relaxationTable.size(); ++i) {
                    const auto& s = results[tableMode].relaxationTable[i];
                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("%zu", i); ImGui::TableSetColumnIndex(1); ImGui::Text("%s", Graph::idToLabel(s.visitedNode).c_str());
                    for (size_t n = 0; n < all.size(); ++n) {
                        ImGui::TableSetColumnIndex((int)n + 2); int node = all[n];
                        if (s.currentDistances.count(node)) {
                            double d = s.currentDistances.at(node);
                            if (d > 1e10) ImGui::Text("inf"); else ImGui::Text("%.0f(%s)", d, Graph::idToLabel(s.predecessors.at(node)).c_str());
                            if (s.visitedNode == node) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0, 0.6f, 0, 0.25f)));
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
    }
    ImGui::End();
    ImGui::Render();
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(isDarkMode ? 0.05f : 1.0f, isDarkMode ? 0.05f : 1.0f, isDarkMode ? 0.05f : 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

int main(int, char**) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CSE373 - Dijkstra Algorithm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 900, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(window);
    IMGUI_CHECKVERSION(); ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context); ImGui_ImplOpenGL3_Init("#version 130");
    cityGraph.loadFromFile("assets/city_map.txt");
    cityGraph.applyCircleLayout(1440/2, 900*0.60/2, 250.0f);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (true) main_loop();
#endif
    return 0;
}
