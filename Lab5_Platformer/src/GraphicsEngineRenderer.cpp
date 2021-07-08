#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "GraphicsEngineRenderer.hpp"

GraphicsEngineRenderer::GraphicsEngineRenderer(int w, int h) : m_screenWidth(w),m_screenHeight(h) {
  	// Initialize random number generation.
   	srand(time(nullptr));

	 // Initialization flag
	 bool success = true;
	 // String to hold any errors that occur.
	 std::stringstream errorStream;
	 // The window we'll be rendering to
	 m_window= nullptr;
	 // Render flag
	
	// Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}else{
		//Create window
		m_window = SDL_CreateWindow( "Lab", 100, 100, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == nullptr ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create a Renderer to draw on
		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
		// Check if Renderer did not create.
		if( m_renderer == nullptr ){
			errorStream << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}
	}

  // If initialization did not work, then print out a list of errors in the constructor.
  if(!success){
    	errorStream << "Failed to initialize Graphics Engine!\n";
    	std::string errors=errorStream.str();
    	std::cout << errors << "\n";
  }else{
    	std::cout << "No SDL errors Detected in during init\n\n";
  }
}

GraphicsEngineRenderer::~GraphicsEngineRenderer(){
    // Destroy Renderer
    SDL_DestroyRenderer(m_renderer);
    //Destroy window
    SDL_DestroyWindow( m_window );
    // Point m_window to nullptr to ensure 
	// it points to nothing.
    m_renderer = nullptr;
    m_window = nullptr;
    
    //Quit SDL subsystems
    SDL_Quit();
}

void GraphicsEngineRenderer::SetRenderDrawColor(int r, int g, int b, int a){
    SDL_SetRenderDrawColor(m_renderer, r,g,b,a);
}

void GraphicsEngineRenderer::RenderClear(){
    SDL_RenderClear(m_renderer);
}

void GraphicsEngineRenderer::RenderPresent(){
    SDL_RenderPresent(m_renderer); 
}

// Get Pointer to Window
SDL_Window* GraphicsEngineRenderer::GetWindow(){
  return m_window;
}

// Get Pointer to Renderer
SDL_Renderer* GraphicsEngineRenderer::GetRenderer(){
  return m_renderer;
}
