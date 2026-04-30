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

#ifdef __EMSCRIPTEN__
#include <SDL2/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#else
#include <SDL.h>
#endif

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

SDL_Window* window = nullptr;
SDL_GLContext gl_context = nullptr;
Graph cityGraph;

char startLabel[16] = "A";
char endLabel[16] = "C";
PathResult results[3]; 
bool pathFound = false;
bool isDarkMode = false;
int tableMode = 0; 

enum class CreationType { ROAD, METRO, BUS };
static CreationType currentCreationType = CreationType::ROAD;

bool showDist = true, showTime = false, showCost = false;
int selectedNode = -1, draggedNode = -1, draggingEdgeFrom = -1;
struct SelectedEdge { int from, to; double dist, time, cost; std::string type; } activeEdge = {-1, -1, 0, 0, 0, "road"};
struct Selection { int from, to; } selectedEdge = {-1, -1};

std::set<int> globallyAvoidedNodes;
std::set<std::string> globallyAvoidedTypes;

#ifdef __EMSCRIPTEN__
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void on_file_uploaded() {
        if (cityGraph.loadFromFile("/uploaded_map.txt")) {
            cityGraph.applyCircleLayout(1440/2, 900*0.55/2, 250.0f);
            pathFound = false;
        }
    }
}
#endif

std::string FileDialog(bool save) {
#if defined(__linux__) && !defined(__EMSCRIPTEN__)
    std::string cmd = save ? "zenity --file-selection --save --title='Save Map' --filename='map.txt' 2>/dev/null" : "zenity --file-selection --title='Load Map' --file-filter='*.txt' 2>/dev/null";
    std::array<char, 128> buffer;
    std::string result = "";
    auto deleter = [](FILE* f) { if (f) pclose(f); };
    std::unique_ptr<FILE, decltype(deleter)> pipe(popen(cmd.c_str(), "r"), deleter);
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) result += buffer.data();
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
#endif
    return "";
}

void TriggerWasmDownload() {
#ifdef __EMSCRIPTEN__
    if (cityGraph.saveToFile("map_save.txt")) {
        emscripten_run_script(R"(
            (function() {
                try {
                    const content = FS.readFile('map_save.txt');
                    const blob = new Blob([content], { type: 'text/plain' });
                    const url = URL.createObjectURL(blob);
                    const a = document.createElement('a');
                    a.href = url;
                    a.download = 'city_map.txt';
                    document.body.appendChild(a);
                    a.click();
                    document.body.removeChild(a);
                    URL.revokeObjectURL(url);
                } catch (e) {
                    console.error("Download failed:", e);
                }
            })();
        )");
    }
#endif
}

void TriggerWasmUpload() {
#ifdef __EMSCRIPTEN__
    emscripten_run_script(R"(
        var input = document.createElement('input');
        input.type = 'file';
        input.accept = '.txt';
        input.onchange = e => {
            var file = e.target.files[0];
            var reader = new FileReader();
            reader.onload = function() {
                var data = new Uint8Array(reader.result);
                FS.writeFile('/uploaded_map.txt', data);
                _on_file_uploaded();
            };
            reader.readAsArrayBuffer(file);
        };
        input.click();
    )");
#else
    std::string p = FileDialog(false);
    if (!p.empty()) {
        if (cityGraph.loadFromFile(p)) {
            cityGraph.applyCircleLayout(1440/2, 900*0.55/2, 250.0f);
            pathFound = false;
        }
    }
#endif
}

void Recompute() {
    int sId = Graph::labelToId(startLabel), eId = Graph::labelToId(endLabel);
    if (sId > 0 && eId > 0) { 
        results[0] = findPath(cityGraph, sId, eId, RouteMode::SHORTEST_DISTANCE, {}, globallyAvoidedNodes, globallyAvoidedTypes); 
        results[1] = findPath(cityGraph, sId, eId, RouteMode::FASTEST_TIME, {}, globallyAvoidedNodes, globallyAvoidedTypes); 
        results[2] = findPath(cityGraph, sId, eId, RouteMode::CHEAPEST_ROUTE, {}, globallyAvoidedNodes, globallyAvoidedTypes); 
        pathFound = true; 
    }
}

void ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    if (isDarkMode) {
        ImGui::StyleColorsDark();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    } else {
        ImGui::StyleColorsLight();
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    }
    style.WindowRounding = 4.0f;
}

float DistToSegment(ImVec2 p, ImVec2 v, ImVec2 w) {
    float l2 = pow(v.x - w.x, 2) + pow(v.y - w.y, 2);
    if (l2 == 0.0) return sqrt(pow(p.x - v.x, 2) + pow(p.y - v.y, 2));
    float t = std::max(0.0f, std::min(1.0f, ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2));
    ImVec2 proj = { v.x + t * (w.x - v.x), v.y + t * (w.y - v.y) };
    return sqrt(pow(p.x - proj.x, 2) + pow(p.y - proj.y, 2));
}

void DrawGraph(ImDrawList* drawList, ImVec2 offset, ImVec2 size) {
    auto& positions = cityGraph.getPositions();
    auto& adj = cityGraph.getAdjList();
    ImVec2 mousePos = ImGui::GetMousePos();
    bool mouseClicked = ImGui::IsMouseClicked(0), mouseDoubleClicked = ImGui::IsMouseDoubleClicked(0);
    ImU32 primaryColor = isDarkMode ? IM_COL32(255, 255, 255, 255) : IM_COL32(0, 0, 0, 255);

    for (auto const& [u, neighbors] : adj) {
        if (globallyAvoidedNodes.count(u)) continue;
        for (auto const& edge : neighbors) {
            if (u > edge.to || globallyAvoidedNodes.count(edge.to) || globallyAvoidedTypes.count(edge.type)) continue;
            ImVec2 p1 = {positions[u].x + offset.x, positions[u].y + offset.y};
            ImVec2 p2 = {positions[edge.to].x + offset.x, positions[edge.to].y + offset.y};
            bool isHighlighted = false;
            if (pathFound && results[tableMode].found) {
                for (size_t i = 0; i < results[tableMode].nodes.size() - 1; ++i) {
                    if ((results[tableMode].nodes[i] == u && results[tableMode].nodes[i+1] == edge.to) || (results[tableMode].nodes[i] == edge.to && results[tableMode].nodes[i+1] == u)) { isHighlighted = true; break; }
                }
            }
            ImU32 col = isDarkMode ? IM_COL32(180, 180, 180, 150) : IM_COL32(80, 80, 80, 150);
            if (edge.type == "road") col = IM_COL32(0, 150, 255, 200);      
            else if (edge.type == "metro") col = IM_COL32(255, 0, 255, 200); 
            else if (edge.type == "bus") col = IM_COL32(255, 165, 0, 200);   
            if (isHighlighted) col = IM_COL32(0, 255, 0, 255);
            else if (selectedEdge.from == u && selectedEdge.to == edge.to) col = IM_COL32(255, 255, 0, 255);
            if (DistToSegment(mousePos, p1, p2) < 12.0f) {
                if (mouseDoubleClicked) { activeEdge = {u, edge.to, edge.distance, edge.time, edge.cost, edge.type}; ImGui::OpenPopup("Edit Edge"); }
                else if (mouseClicked) { selectedEdge = {u, edge.to}; selectedNode = -1; }
            }
            drawList->AddLine(p1, p2, col, isHighlighted ? 6.0f : 3.0f);
            if (showDist) { ImVec2 mid = {(p1.x+p2.x)/2, (p1.y+p2.y)/2}; drawList->AddText(mid, primaryColor, std::to_string((int)edge.distance).c_str()); }
        }
    }
    if (draggingEdgeFrom != -1) {
        ImVec2 p1 = {positions[draggingEdgeFrom].x + offset.x, positions[draggingEdgeFrom].y + offset.y};
        ImU32 wireCol = IM_COL32(200, 200, 200, 200);
        if (currentCreationType == CreationType::ROAD) wireCol = IM_COL32(0, 150, 255, 255);
        else if (currentCreationType == CreationType::METRO) wireCol = IM_COL32(255, 0, 255, 255);
        else if (currentCreationType == CreationType::BUS) wireCol = IM_COL32(255, 165, 0, 255);
        drawList->AddLine(p1, mousePos, wireCol, 2.5f);
    }
    for (auto& [id, pos] : positions) {
        ImVec2 p = {pos.x + offset.x, pos.y + offset.y};
        float radius = 22.0f;
        bool isAvoided = globallyAvoidedNodes.count(id);
        bool hovered = sqrt(pow(mousePos.x - p.x, 2) + pow(mousePos.y - p.y, 2)) < radius;
        if (hovered && ImGui::IsMouseDown(0) && draggingEdgeFrom == -1 && draggedNode == -1) draggedNode = id;
        if (hovered && ImGui::IsMouseDown(1) && draggingEdgeFrom == -1) draggingEdgeFrom = id;
        if (draggedNode == id) { pos.x = mousePos.x - offset.x; pos.y = mousePos.y - offset.y; }
        if (hovered && mouseClicked) { selectedNode = id; selectedEdge = {-1, -1}; }
        ImU32 circleColor = (isDarkMode ? IM_COL32(50, 50, 60, 255) : IM_COL32(240, 240, 240, 255));
        std::string label = Graph::idToLabel(id);
        if (selectedNode == id) circleColor = IM_COL32(255, 255, 0, 255);
        if (label == startLabel) circleColor = IM_COL32(0, 200, 0, 255); 
        else if (label == endLabel) circleColor = IM_COL32(0, 200, 200, 255); 
        drawList->AddCircleFilled(p, radius, circleColor);
        ImU32 borderCol = isAvoided ? IM_COL32(255, 0, 0, 255) : primaryColor;
        drawList->AddCircle(p, radius, borderCol, 0, isAvoided ? 4.0f : 2.0f);
        if (isAvoided) { float s = radius*0.5f; drawList->AddLine({p.x-s,p.y-s},{p.x+s,p.y+s},borderCol,3.0f); drawList->AddLine({p.x+s,p.y-s},{p.x-s,p.y+s},borderCol,3.0f); }
        drawList->AddText({p.x - ImGui::CalcTextSize(label.c_str()).x/2, p.y - ImGui::CalcTextSize(label.c_str()).y/2}, (selectedNode==id)?IM_COL32(0,0,0,255):primaryColor, label.c_str());
    }
    if (!ImGui::IsMouseDown(1)) {
        if (draggingEdgeFrom != -1) {
            for (auto& [id, pos] : positions) {
                if (sqrt(pow(mousePos.x - (pos.x+offset.x), 2) + pow(mousePos.y - (pos.y+offset.y), 2)) < 30.0f && id != draggingEdgeFrom) { 
                    std::string t = (currentCreationType == CreationType::ROAD) ? "road" : (currentCreationType == CreationType::METRO ? "metro" : "bus");
                    cityGraph.addEdge(draggingEdgeFrom, id, 10.0, 5.0, 2.0, t); break; 
                }
            }
        }
        draggingEdgeFrom = -1;
    }
    if (!ImGui::IsMouseDown(0)) draggedNode = -1;
    if (ImGui::BeginPopupModal("Edit Edge", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputDouble("Dist", &activeEdge.dist); ImGui::InputDouble("Time", &activeEdge.time); ImGui::InputDouble("Cost", &activeEdge.cost);
        const char* tNames[] = {"road", "metro", "bus"}; static int tIdx = 0;
        for(int i=0; i<3; i++) if(activeEdge.type == tNames[i]) tIdx = i;
        if(ImGui::Combo("Type", &tIdx, tNames, 3)) activeEdge.type = tNames[tIdx];
        if (ImGui::Button("Update")) { cityGraph.updateEdge(activeEdge.from, activeEdge.to, activeEdge.dist, activeEdge.time, activeEdge.cost, activeEdge.type); ImGui::CloseCurrentPopup(); }
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
    ImGui::SetNextWindowPos(ImVec2(0, 0)); ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Site", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    float canvasH = ImGui::GetIO().DisplaySize.y * 0.55f, canvasW = ImGui::GetIO().DisplaySize.x;
    ImGui::BeginChild("GraphArea", ImVec2(0, canvasH), true, ImGuiWindowFlags_NoScrollbar);
    ImVec2 childPos = ImGui::GetCursorScreenPos(); DrawGraph(ImGui::GetWindowDrawList(), childPos, ImVec2(canvasW, canvasH));
    ImGui::SetCursorScreenPos(ImVec2(childPos.x + 10, childPos.y + 10)); 
    ImGui::BeginGroup();
    if (ImGui::Button(isDarkMode ? "LIGHT MODE" : "DARK MODE", ImVec2(140, 30))) isDarkMode = !isDarkMode;
    ImGui::Spacing(); ImGui::TextColored(isDarkMode?ImVec4(0,1,0,1):ImVec4(0,0.5f,0,1), "PATH SETUP");
    ImGui::PushItemWidth(140); ImGui::InputText("Start", startLabel, 16); ImGui::InputText("End", endLabel, 16); ImGui::PopItemWidth();
    if (ImGui::Button("COMPUTE", ImVec2(140, 35))) { Recompute(); }
    ImGui::Spacing(); if (ImGui::Button("ADD NODE", ImVec2(140, 30))) { int mId = 0; for (auto id : cityGraph.getAllNodes()) mId = std::max(mId, id); cityGraph.setNodePos(mId + 1, 200+(rand()%400), 100+(rand()%200)); }
    if (selectedNode != -1 && ImGui::Button("DELETE NODE", ImVec2(140, 30))) { cityGraph.removeNode(selectedNode); selectedNode = -1; pathFound = false; }
    ImGui::Spacing(); ImGui::TextColored(isDarkMode?ImVec4(1,1,0,1):ImVec4(0.5f,0.5f,0,1), "CREATION TYPE");
    ImGui::RadioButton("Road", (int*)&currentCreationType, 0); ImGui::RadioButton("Metro", (int*)&currentCreationType, 1); ImGui::RadioButton("Bus", (int*)&currentCreationType, 2);
    ImGui::EndGroup();
    ImGui::SetCursorScreenPos(ImVec2(childPos.x + canvasW - 160, childPos.y + 10)); 
    ImGui::BeginGroup();
    if (ImGui::Button("IMPORT", ImVec2(140, 30))) { ImGui::OpenPopup("Select Map"); }
    if (ImGui::Button("STORE", ImVec2(140, 30))) {
#ifdef __EMSCRIPTEN__
        TriggerWasmDownload();
#else
        std::string p = FileDialog(true); if (!p.empty()) cityGraph.saveToFile(p);
#endif
    }
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0, 0, 1));
    if (ImGui::Button("CLEAR ALL", ImVec2(140, 30))) { cityGraph.getAdjList().clear(); cityGraph.getPositions().clear(); globallyAvoidedNodes.clear(); globallyAvoidedTypes.clear(); pathFound = false; startLabel[0] = 'A'; startLabel[1] = '\0'; endLabel[0] = 'C'; endLabel[1] = '\0'; }
    ImGui::PopStyleColor();
    ImGui::Spacing(); ImGui::TextColored(isDarkMode?ImVec4(1,0.4f,0.4f,1):ImVec4(0.8f,0,0,1), "NODE FILTERS");
    ImGui::BeginChild("NF", ImVec2(140, 90), true);
    auto allN = cityGraph.getAllNodes(); std::sort(allN.begin(), allN.end());
    for (int n : allN) { bool act = !globallyAvoidedNodes.count(n); if (ImGui::Checkbox(Graph::idToLabel(n).c_str(), &act)) { if(!act) globallyAvoidedNodes.insert(n); else globallyAvoidedNodes.erase(n); if(pathFound) Recompute(); } }
    ImGui::EndChild();
    ImGui::Spacing(); ImGui::TextColored(isDarkMode?ImVec4(1,0.4f,1,1):ImVec4(0.6f,0,0.6f,1), "TRANSPORT FILTERS");
    auto TC = [&](const char* l, const char* t, ImVec4 c) {
        std::string typeStr = t;
        bool act = !globallyAvoidedTypes.count(typeStr); ImGui::PushStyleColor(ImGuiCol_Text, c);
        if (ImGui::Checkbox(l, &act)) {
            if (!act) globallyAvoidedTypes.insert(typeStr);
            else globallyAvoidedTypes.erase(typeStr);
            if (pathFound) Recompute();
        }
        ImGui::PopStyleColor();
    };
    TC("Road Filter", "road", ImVec4(0,0.6f,1,1)); TC("Metro Filter", "metro", ImVec4(1,0,1,1)); TC("Bus Filter", "bus", ImVec4(1,0.5f,0,1));
    ImGui::EndGroup();
    if (ImGui::BeginPopup("Select Map")) {
#ifdef __EMSCRIPTEN__
        const char* maps[] = { "/assets/city_map.txt", "/assets/circular_map.txt", "/assets/grid_map.txt", "/assets/test.txt" };
#else
        const char* maps[] = { "assets/city_map.txt", "assets/circular_map.txt", "assets/grid_map.txt", "assets/test.txt" };
#endif
        for (int i = 0; i < 4; i++) if (ImGui::Selectable(maps[i])) { if (cityGraph.loadFromFile(maps[i])) { cityGraph.applyCircleLayout(canvasW/2, canvasH/2, 200.0f); pathFound = false; } }
        ImGui::Separator();
        if (ImGui::Selectable("UPLOAD FROM PC (.txt)")) { TriggerWasmUpload(); }
        ImGui::EndPopup();
    }
    ImGui::EndChild();
    ImGui::Columns(2, "Bottom", true); ImGui::SetColumnWidth(0, 450.0f);
    {
        ImGui::TextColored(isDarkMode?ImVec4(0,1,0,1):ImVec4(0,0.6f,0,1), "RESULTS SUMMARY");
        ImGui::Checkbox("D", &showDist); ImGui::SameLine(); ImGui::Checkbox("T", &showTime); ImGui::SameLine(); ImGui::Checkbox("C", &showCost);
        if (pathFound) {
            bool anyF = false;
            if (ImGui::BeginTable("Res", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Mode"); ImGui::TableSetupColumn("D"); ImGui::TableSetupColumn("T"); ImGui::TableSetupColumn("C"); ImGui::TableSetupColumn("ms");
                ImGui::TableHeadersRow();
                for (int i=0; i<3; ++i) {
                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("%s", i==0?"Short":i==1?"Fast":"Cheap");
                    if (results[i].found) { anyF = true; ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", results[i].totalDistance); ImGui::TableSetColumnIndex(2); ImGui::Text("%.0f", results[i].totalTime); ImGui::TableSetColumnIndex(3); ImGui::Text("%.0f", results[i].totalCost); ImGui::TableSetColumnIndex(4); ImGui::TextColored(ImVec4(1,0.5f,0,1), "%.2f", results[i].executionTimeMs); } else ImGui::Text("N/A");
                }
                ImGui::EndTable();
            }
            if (!anyF) ImGui::TextColored(ImVec4(1,0,0,1), "No path found from %s to %s!", startLabel, endLabel);
            else { for(int i=0; i<3; i++) if(results[i].found) { ImGui::TextColored(isDarkMode?ImVec4(0.5f,1,0.5f,1):ImVec4(0,0.5f,0,1), "%s Path:", i==0?"Shortest":i==1?"Fastest":"Cheapest"); std::string p = ""; for(size_t n=0; n<results[i].nodes.size(); n++) p += Graph::idToLabel(results[i].nodes[n]) + (n==results[i].nodes.size()-1?"":" -> "); ImGui::TextWrapped("%s", p.c_str()); } }
        }
    }
    ImGui::NextColumn();
    {
        ImGui::TextColored(isDarkMode?ImVec4(1,0.8f,0,1):ImVec4(0.7f,0.5f,0,1), "RELAXATION TRACE");
        const char* mNs[] = {"Distance", "Time", "Cost"}; ImGui::SetNextItemWidth(120); ImGui::Combo("Metric", &tableMode, mNs, 3);
        if (pathFound && results[tableMode].found) {
            auto nodes = cityGraph.getAllNodes(); std::sort(nodes.begin(), nodes.end());
            if (ImGui::BeginTable("Trace", (int)nodes.size() + 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX, ImVec2(0, 150))) {
                ImGui::TableSetupColumn("Step"); ImGui::TableSetupColumn("Node"); for (int n : nodes) ImGui::TableSetupColumn(Graph::idToLabel(n).c_str(), ImGuiTableColumnFlags_WidthFixed, 45); ImGui::TableHeadersRow();
                for (size_t i = 0; i < results[tableMode].relaxationTable.size(); i++) {
                    const auto& s = results[tableMode].relaxationTable[i]; ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0); ImGui::Text("%zu", i); ImGui::TableSetColumnIndex(1); ImGui::Text("%s", Graph::idToLabel(s.visitedNode).c_str());
                    for (size_t n = 0; n < nodes.size(); n++) {
                        int nid = nodes[n]; ImGui::TableSetColumnIndex((int)n + 2);
                        if (s.currentDistances.count(nid)) {
                            double d = s.currentDistances.at(nid);
                            if (d > 1e10) ImGui::Text("inf"); else {
                                if (s.predecessors.count(nid) && s.predecessors.at(nid) != -1) ImGui::Text("%.0f(%s)", d, Graph::idToLabel(s.predecessors.at(nid)).c_str());
                                else ImGui::Text("%.0f", d);
                            }
                            if (s.visitedNode == nid) ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImVec4(0, 0.6f, 0, 0.3f)));
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
    glClearColor(isDarkMode ? 0.05f : 0.95f, isDarkMode ? 0.05f : 0.95f, isDarkMode ? 0.05f : 0.95f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); SDL_GL_SwapWindow(window);
}

int main(int, char**) {
    SDL_Init(SDL_INIT_VIDEO);
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    window = SDL_CreateWindow("dijkstra-algorithm", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1440, 900, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    gl_context = SDL_GL_CreateContext(window); IMGUI_CHECKVERSION(); ImGui::CreateContext(); ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
#ifdef __EMSCRIPTEN__
    ImGui_ImplOpenGL3_Init("#version 300 es");
#else
    ImGui_ImplOpenGL3_Init("#version 130");
#endif
#ifdef __EMSCRIPTEN__
    cityGraph.loadFromFile("/assets/city_map.txt");
#else
    cityGraph.loadFromFile("assets/city_map.txt");
#endif
    cityGraph.applyCircleLayout(1440/2, 900*0.55/2, 250.0f);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (true) main_loop();
#endif
    return 0;
}
