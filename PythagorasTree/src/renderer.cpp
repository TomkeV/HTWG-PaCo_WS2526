#include <iostream>
#include "renderer.hpp"

#include <omp.h>
#include <unordered_set>
#include <vector>
#include <utility>

// 
RenderObject arrow_X_Axis = {
    // Vertices
    {
        { -5.0f, 0.0f, 0.0f },
        {  5.0f, 0.0f, 0.0f },
        {  4.5f,  0.2f, 0.2f },
        {  4.5f, -0.2f, 0.2f },
        {  4.5f, -0.2f, -0.2f },
        {  4.5f,  0.2f, -0.2f }
    },
    // Edges
    {
        { 0, 1 }, // Main line
        { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, // Arrowhead
        { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 2 }  // Arrowhead base
    }, 
    createVector3dFromValues(1.0, 0.0, 0.0) // color
};

RenderObject arrow_Y_Axis = {
    // Vertices
    {
        { 0.0f, -5.0f, 0.0f },
        { 0.0f,  5.0f, 0.0f },
        { 0.2f,  4.5f, 0.2f },
        { -0.2f, 4.5f, 0.2f },
        { -0.2f, 4.5f, -0.2f },
        { 0.2f,  4.5f, -0.2f }
    },
    // Edges
    {
        { 0, 1 }, // Main line
        { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, // Arrowhead
        { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 2 }  // Arrowhead base
    },
    createVector3dFromValues(0.0, 1.0, 0.0) // color
};

RenderObject arrow_Z_Axis = {
    // Vertices
    {
        { 0.0f, 0.0f, -5.0f },
        { 0.0f, 0.0f,  5.0f },
        { 0.2f,  0.2f, 4.5f },
        { -0.2f, 0.2f, 4.5f },
        { -0.2f, -0.2f, 4.5f },
        { 0.2f,  -0.2f, 4.5f }
    },
    // Edges
    {
        { 0, 1 }, // Main line
        { 1, 2 }, { 1, 3 }, { 1, 4 }, { 1, 5 }, // Arrowhead
        { 2, 3 }, { 3, 4 }, { 4, 5 }, { 5, 2 }  // Arrowhead base
    },
    createVector3dFromValues(0.0, 0.0, 1.0) // color
};

// private functions
void Renderer::initCamera() {
	Camera c;
    c.position = { 2.0f, -20.0f, 10.0f };
    c.target = { 0.0f, 0.0f, 5.0f };
    c.up = { 0.0f, 0.0f, 1.0f };
    c.fovYRadians = 60.0f * 3.14159265f / 180.0f;
    c.aspect = double(this->screenWidth) / double(this->screenHeight);
    c.zNear = 0.1f;
    c.zFar = 100.0f;
    this->cam = c;
}

void Renderer::initWireframeRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        this->window = nullptr;
        this->renderer = nullptr;
    }

    SDL_Window* window = SDL_CreateWindow(
        "3D Wireframe Cube",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        this->screenWidth,
        this->screenHeight,
        SDL_WINDOW_HIDDEN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        SDL_Quit();
        this->window = nullptr;
        this->renderer = nullptr;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        this->window = nullptr;
        this->renderer = nullptr;
    }

    this->window = window;
    this->renderer = renderer;
}

void Renderer::deinitWireframeRenderer() {
    if (this->renderer) SDL_DestroyRenderer(this->renderer);
    if (this->window) SDL_DestroyWindow(this->window);
    SDL_Quit();
}

// Rule of five
// Destructor
Renderer::~Renderer() {
    deinitWireframeRenderer();
    // Camera?
    // other Attributes?
}

// Constructors
Renderer::Renderer() {
    this->screenHeight = 800;
    this->screenWidth = 1000;
    this->running = false;
    this->renderObjects = std::vector<RenderObject>();
    initWireframeRenderer();
   	initCamera();
}

// functions
void Renderer::renderObject(const RenderObject& obj) {
    // Reduziere die Kanten, bevor wir das Objekt rendern
    std::vector<std::pair<int, int>> edges = obj.edges;
    removeRedundantEdges(edges);

    std::vector<ScreenPoint> projected(obj.verts.size());
    std::vector<bool> visible(obj.verts.size(), false);
    size_t vertexCount = obj.verts.size();

    // Projektion und Sichtbarkeitspr�fung
    for (size_t i = 0; i < vertexCount; ++i) {
        if (auto sp = projectPoint(obj.verts[i], this->cam, this->screenWidth, this->screenHeight)) {
            projected[i] = *sp;
            visible[i] = true;
        }
    }

    // Setze die Zeichnungsfarbe 
    SDL_SetRenderDrawColor(this->renderer,
        static_cast<Uint8>(obj.color.x_ * 255),
        static_cast<Uint8>(obj.color.y_ * 255),
        static_cast<Uint8>(obj.color.z_ * 255),
        255);

    // Zeichne nur die sichtbaren Kanten
    for (const auto& [a, b] : edges) {
        if (visible[a] && visible[b]) {
            SDL_RenderDrawLine(
                renderer,
                static_cast<int>(projected[a].x), static_cast<int>(projected[a].y),
                static_cast<int>(projected[b].x), static_cast<int>(projected[b].y)
            );
        }
    }
}

void Renderer::rotateAroundZ(Vector3D& v, float angleRadians) {
    float s = std::sin(angleRadians);
    float c = std::cos(angleRadians);

    float xNew = v.x_ * c - v.y_ * s;
    float yNew = v.x_ * s + v.y_ * c;

    v.x_ = xNew;
    v.y_ = yNew;
}

void Renderer::pollEvents(bool keys[4]) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            this->running = false;
        }
        else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_a) {
                keys[0] = true;
            }
            if (e.key.keysym.sym == SDLK_d) {
                keys[1] = true;
            }
            if (e.key.keysym.sym == SDLK_w) {
                keys[2] = true;
            }
            if (e.key.keysym.sym == SDLK_s) {
                keys[3] = true;
            }
        }
        else if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == SDLK_a) {
                keys[0] = false;
            }
            if (e.key.keysym.sym == SDLK_d) {
                keys[1] = false;
            }
            if (e.key.keysym.sym == SDLK_w) {
                keys[2] = false;
            }
            if (e.key.keysym.sym == SDLK_s) {
                keys[3] = false;
            }
        }
    }
}

void Renderer::addRenderObject(RenderObject&& r) {
    this->renderObjects.push_back(std::move(r));
}

/* Entfernung von Kanten, die aufeinander liegen -> Effizienzsteigerung
*/
namespace std {
    // Benutzerdefinierter Hash f�r die std::pair
    template <>
    struct hash<std::pair<int, int>> {
        size_t operator()(const std::pair<int, int>& edge) const {
            return std::hash<int>()(edge.first) ^ std::hash<int>()(edge.second);
        }
    };
}

void Renderer::removeRedundantEdges(std::vector<std::pair<int, int>>& edges) {
    std::unordered_set<std::pair<int, int>> uniqueEdgesSet;
    std::vector<std::pair<int, int>> uniqueEdges;

    for (const auto& edge : edges) {
        int a = std::min(edge.first, edge.second);
        int b = std::max(edge.first, edge.second);
        std::pair<int, int> sortedEdge = { a, b };

        // �berpr�fe und f�ge die Kante zur Menge hinzu
        if (uniqueEdgesSet.insert(sortedEdge).second) {
            uniqueEdges.emplace_back(sortedEdge);
        }
    }

    // Ersetze die Kantenliste mit den einzigartigen Kanten
    edges = std::move(uniqueEdges);
}

void Renderer::run() {
    // Show window
    SDL_ShowWindow(this->window);

    Uint32 lastTicks = SDL_GetTicks();
    bool keys[4] = { false, false, false, false }; // A, D, W, S
    this->running = true;

    this->viewMatrix = this->cam.viewMatrix();
    this->projMatrix = this->cam.projMatrix();

    while (this->running) {
        pollEvents(keys);

        Uint64 ticks = SDL_GetTicks64();
        double dt = (ticks - lastTicks) / 1000.0;
        lastTicks = ticks;

        if (keys[0]) {
            this->cam.position.x_ -= 1.5 * dt;
        }
        if (keys[1]) {
            this->cam.position.x_ += 1.5 * dt;
        }
        if (keys[2]) {
            this->cam.position.z_ += 1.5 * dt;
        }
        if (keys[3]) {
            this->cam.position.z_ -= 1.5 * dt;
        }

        // Clear screen (black)
        // SDL_SetRenderDrawColor(this->renderer, 255, 255, 255, 255); // white background
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 255); // black background
        SDL_RenderClear(this->renderer);

        // render coordinate System
        /*renderObject(arrow_X_Axis); 
        renderObject(arrow_Y_Axis);
        renderObject(arrow_Z_Axis);*/


// #pragma omp parallel for // ToDo! Geht nicht mehr
        for (int i = 0; i < this->renderObjects.size(); ++i) {
            renderObject(this->renderObjects[i]);
        }
        SDL_RenderPresent(renderer);
    }
}