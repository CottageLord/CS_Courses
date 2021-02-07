#ifndef PLAYER_SCORE_CLASS
#define PLAYER_SCORE_CLASS

#include "config.hpp"
#include "Vec2.hpp"

// for displaying player scores
class PlayerScore
{
public:
	// specified initializer
	PlayerScore(Vec2 position, SDL_Renderer* renderer, TTF_Font* font)
		: renderer(renderer), font(font)
	{
		// the surfaces to be written
		surface = TTF_RenderText_Solid(font, "0", {0xFF, 0xFF, 0xFF, 0xFF});
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

		rect.x = static_cast<int>(position.x);
		rect.y = static_cast<int>(position.y);
		rect.w = width;
		rect.h = height;
	}

	~PlayerScore()
	{
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
	}

	void Draw()
	{
		SDL_RenderCopy(renderer, texture, nullptr, &rect);
	}

	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Surface* surface{};
	SDL_Texture* texture{};
	SDL_Rect rect{};
};

extern PlayerScore score_display;

#endif