#ifndef SDLGRAPHICSPROGRAM
#define SDLGRAPHICSPROGRAM

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <vector>
// I recommend a map for filling in the resource manager
#include <map>
#include <memory>
#include <iterator>

#include "GraphicsEngineRenderer.hpp"
#include "Component.hpp"
#include "TransformComponent.hpp"
#include "ControllerComponent.hpp"
#include "SpriteComponent.hpp"
#include "GameObject.hpp"
#include "TileMap.hpp"

/**
 * This class sets up the main game engine
 */
class Engine{
public:
    /**
     * Constructor of Engine
     */
    Engine();
    /**
     * Destructor
     */
    ~Engine();
    /**
     * Input engine
     */
    void Input();
    /**
     * Per frame update
     */
    void Update();
    /**
     * Per frame render. Renders everything
     */
    void Render();
    /**
     *  A simple frame stablizer
     */
    void update_with_timer(std::chrono::steady_clock::time_point &previous_time, 
        double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update);
    /**
     * Main Game Loop that runs forever
     */
    void MainGameLoop();
    /**
     * Initialization and shutdown pattern
     * Explicitly call 'Start' to launch the engine
     */
    void Start();
    /**
     * Initialization and shutdown pattern
     * Explicitly call 'Shutdown' to terminate the engine
     */
    void Shutdown();

    /**
     * Request to startup the Graphics Subsystem
     */
	void InitializeGraphicsSubSystem();


private:
    // Engine Subsystem
    // Setup the Graphics Rendering Engine
    GraphicsEngineRenderer* m_renderer = nullptr;
    std::vector<GameObject*> m_objects;
    SDL_Event SDLEvent;

};

#endif
