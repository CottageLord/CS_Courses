/** @file Text_display.hpp
@author yang hu
@version 1.0
@brief The text drawer to display various massage to players
@date Monday, Feburuary 22, 2021
*/
#ifndef TEXT_DISPLAY_CLASS
#define TEXT_DISPLAY_CLASS

#include "config.hpp"
#include "Vec2.hpp"

/**@class Text_display
 *@brief Class for object to display text at fixed position
*/
class Text_display
{
public:

	SDL_Renderer* renderer; ///< Reference to SDL renderer
	TTF_Font* font; ///< Reference to the font object we loaded
	SDL_Surface* surface{}; ///< Reference to SDL text surface
	SDL_Texture* texture{}; ///< Reference to SDL text texture
	SDL_Rect rect{}; ///< The box area that will contain the generated text area
	
	/// @brief Initialize a new text display at a given position
	Text_display(Vec2 position, SDL_Renderer* renderer, TTF_Font* font)
		: renderer(renderer), font(font)
	{
		surface = TTF_RenderText_Solid(font, "0", {0xFF, 0xFF, 0xFF, 0xFF});
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}

	/// @brief update the text that will be displayed
	void set_text(std::string const &text_to_display)
	{
		// free the former surface
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		surface = TTF_RenderText_Solid(font, text_to_display.c_str(), {0xFF, 0xFF, 0xFF, 0xFF});
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
		rect.w = width;
		rect.h = height;
	}

	/// @brief Offload the text resources, surfaces, textures
	~Text_display()
	{
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	/// @brief Generate draw information to renderer
	void Draw()
	{	
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}
};

#endif