#ifndef GRAPHICS_ENGINE_RENDERER_HPP
#define GRAPHICS_ENGINE_RENDERER_HPP


#include "config.hpp"
/**
 * This class serves as an interface to
 * the main graphics renderer for our engine.
 */
class GraphicsEngineRenderer{
    public:
        /**
         * Constructor
         */
        GraphicsEngineRenderer(int w, int h);
        /**
         * Destructor
         */
        ~GraphicsEngineRenderer();
        /**
         * Set the color for the background whenever
         * the color is cleared.
         */
        void SetRenderDrawColor(int r, int g, int b, int a);
        /**
         * Clear the screen
         */
        void RenderClear();
        /**
         * Render whatever is in the backbuffer to
         * the screen.
         */
        void RenderPresent();
        /**
         * Get Pointer to Window
         */
        SDL_Window* GetWindow();
        /**
         * Get Pointer to Renderer
         */
        SDL_Renderer* GetRenderer();

    private:
        // Screen dimension constants
		int m_screenHeight;
		int m_screenWidth;
        // SDL Window
        SDL_Window* m_window;
        // SDL Renderer
        SDL_Renderer* m_renderer = nullptr;
};

#endif
