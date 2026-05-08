#pragma once

#define SDL_MAIN_HANDLED

#ifdef __linux__
    #include <SDL2/SDL.h>
#else
    #include <SDL.h>
#endif
#include "graphics_structs.hpp"

// Coordinate System
extern RenderObject arrow_X_Axis;
extern RenderObject arrow_Y_Axis;
extern RenderObject arrow_Z_Axis;

// Colours
static const Vector3D BROWN_DARK = createVector3dFromValues(0.35, 0.2, 0.008);
static const Vector3D BROWN_STANDARD = createVector3dFromValues(0.6, 0.337, 0);
static const Vector3D BROWN_LIGHT = createVector3dFromValues(0.6, 0.455, 0.196);
static const Vector3D GREEN_LIGHT = createVector3dFromValues(0.184, 0.741, 0.231);
static const Vector3D GREEN_STANDARD = createVector3dFromValues(0.133, 0.588, 0.169);
static const Vector3D GREEN_DARK = createVector3dFromValues(0.024, 0.471, 0.063);

/** ToDo.
 *
 */
class Renderer {
public:
	// Rule of five:
	Renderer(const Renderer&) = default;			// Copy-Constructor
	Renderer(Renderer&&) = default;					// Move-Constructor
	Renderer& operator=(const Renderer&) = default; // Copy-Zuweisung
	Renderer& operator=(Renderer&&) = default;		// Move-Zuweisung
	~Renderer();									// Destructor

	// further constructors
	Renderer(); // default constructor

	// functions
	void renderObject(const RenderObject&);
	void rotateAroundZ(Vector3D&, float);
    void pollEvents(bool keys[4]);
    void addRenderObject(RenderObject&&);
    void run();

    void removeRedundantEdges(std::vector<std::pair<int, int>>&);

private:
	// Attributes
	Camera cam;
	int screenWidth;
	int screenHeight;
	bool running;
	SDL_Window* window;
	SDL_Renderer* renderer;
    std::vector<RenderObject> renderObjects;

    Mat4 viewMatrix;
    Mat4 projMatrix;

	// Functions
	void initCamera();
	void initWireframeRenderer();
	void deinitWireframeRenderer();
};