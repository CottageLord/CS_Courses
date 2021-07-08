/** @file Resource_manager.hpp
@author yang hu
@version 1.0
@brief A singleton resource manager that manages game resources
@details resource includes: font, brick objects, sound and textures
@date Monday, Feburuary 22, 2021
*/
#ifndef RESOURCE_MANAGER_CLASS
#define RESOURCE_MANAGER_CLASS

#include <fstream>
#include "config.hpp"
#include "Brick.hpp"
#include "Text_display.hpp"

/**@class Resource_manager
 *@brief defines resource loading, managig and offloading
*/
class Resource_manager {
	
private:
	/// @brief avoid public initialization - Singleton
	Resource_manager(Text_display *player_1, Text_display *player_2) { }
	/// @brief Avoid copy constructor
	Resource_manager(Resource_manager const&);
	/// @brief Don't allow assignment.
    void operator=(Resource_manager const&); 
	static Resource_manager* instance; ///< initialize the singleton pointer field

public:
	std::vector<std::vector<Brick>> level_bricks; ///< data structure to store all bricks
	std::vector<std::string> text_display; ///< stores referece to all text displayer
	Text_display *display_1; ///< text display for score, lives
	Text_display *display_2; ///< text display for messages like win/lose
	// Initialize sound effects
	Mix_Chunk* wall_hit_sound; ///< sound resource for wall hit sound
	Mix_Chunk* paddle_hit_sound; ///< sound resource for paddle hit sound
	Mix_Music* background_music; ///< sound resource for bgm
	// Initialize the font
	TTF_Font* score_font; ///< font resource for display text

	/// @brief get the instance of the resource manager, create one if not instanciated
	static Resource_manager *get_instance() {
		if (!instance) instance = new Resource_manager(0,0);
		return instance;
	}

	/// @brief load all resources, happens only once at the beginning of the game
	void load_resources(SDL_Renderer* renderer) {
		// load sound resources
	    wall_hit_sound   = Mix_LoadWAV(WALL_SOUND_FILE);
	    paddle_hit_sound = Mix_LoadWAV(PADDLE_SOUND_FILE);
	    background_music = Mix_LoadMUS(BGM_FILE);
	    // load font
	    score_font = TTF_OpenFont(FONT_FILE, 40);
	    // score display
    	display_1 = new Text_display(Vec2(SCREEN_WIDTH / 4, 0), renderer, score_font);
    	// notification text display
    	display_2 = new Text_display(Vec2( 100, SCREEN_HEIGHT / 2), renderer, score_font);
    
	}
	/// @brief load localization text after the player has chose language
	void load_text(const std::string& filename) {
		// open the file
		std::ifstream localization_text(filename);

		if (!localization_text) {
	        // Print an error and exit
	        std::cerr << "Cannot open the level data" << std::endl;
	        return;
	    }
		// vector of bricks
	    std::string input;
	    // store all bricks
	    while (std::getline(localization_text, input)) {
	    	text_display.insert(text_display.end(), input);
	    }
	}
	/// @brief load all levels from files, store in 2d vector
	void load_level(const std::string& filename) {
		// open the file
		std::ifstream level_data(filename);

		if (!level_data) {
	        // Print an error and exit
	        std::cerr << "Cannot open the level data" << std::endl;
	        return;
	    }
		// vector of bricks
	    std::vector<Brick> row;
	    std::string input;
	    // store all bricks
	    while (level_data >> input) {
	    	row.clear();
	        for (char i : input) {
	        	Brick *new_brick = new Brick(Vec2(),Vec2());
	            if(i == EMPTY_BRICK) new_brick->status = Brick_type::None;
	            if(i == REAL_BRICK ) new_brick->status = Brick_type::Exist;
	            row.push_back(*new_brick);
	            //delete new_brick;
	        }
	        level_bricks.insert(level_bricks.begin(), row);
	    }
	    // how many rows/cols of bricks do we need
	    int row_n = level_bricks.size();
	    int col_n = level_bricks[0].size();
	    // calculate brick stats
	    float brick_width  = 
	    	(SCREEN_WIDTH - 2 * BRICK_SIDE_PADDING - (col_n - 1) * BRICK_GAP) / col_n;
	    float brick_height = 
	    	(BRICK_AREA - (row_n - 1) * BRICK_GAP) / row_n;
	    // fix all bricks pos
	    float curr_brick_pos_x = BRICK_SIDE_PADDING;
	    float curr_brick_pos_y = BRICK_TOP_PADDING;
	    // iterate through all bricks
	    bricks_remained = 0;
	    for (int i = level_bricks.size() - 1; i >= 0; i--)
	    {
	        for (int j = 0; j < level_bricks[0].size(); j++)
	        {
	            //if a valid brick, calculate its position
	            if (level_bricks[i][j].status != Brick_type::None)
	            {
	            	level_bricks[i][j].position.x = curr_brick_pos_x;
		            level_bricks[i][j].position.y = curr_brick_pos_y;
		            level_bricks[i][j].rect.x 	  = curr_brick_pos_x;
		            level_bricks[i][j].rect.y 	  = curr_brick_pos_y;
		            level_bricks[i][j].rect.w 	  = brick_width;
		            level_bricks[i][j].rect.h 	  = brick_height;
		            // record brick number
		            bricks_remained++;
		        }
	            curr_brick_pos_x = curr_brick_pos_x + brick_width + BRICK_GAP;
	        }
	        // draw next row
	        curr_brick_pos_x = BRICK_SIDE_PADDING;
	        curr_brick_pos_y = curr_brick_pos_y + brick_height + BRICK_GAP;
	    }
	}
	/// @brief empty deconstructor
	~Resource_manager(){ }

	/// @brief offload all loaded resources
	void destroy() {
		delete display_1;
		delete display_2;
		TTF_CloseFont(score_font);
	    Mix_FreeChunk(wall_hit_sound);
	    Mix_FreeChunk(paddle_hit_sound);
	    Mix_FreeMusic(background_music);
		level_bricks.clear();
	}
};

#endif