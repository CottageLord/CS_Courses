#ifndef GRAPHICS_ENGINE_RENDERER_HPP
#define GRAPHICS_ENGINE_RENDERER_HPP
// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
//
// Note that your path may be different depending on where you intalled things
//
//
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

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
