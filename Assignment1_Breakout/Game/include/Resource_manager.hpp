#ifndef RESOURCE_MANAGER_CLASS
#define RESOURCE_MANAGER_CLASS

#include <fstream>
#include "config.hpp"
#include "Brick.hpp"
#include "PlayerScore.hpp"

// This is for holding score display pointers
const char EMPTY_BRICK = '0';
const char REAL_BRICK  = '1';

class Resource_manager {
	
private:
	Resource_manager(PlayerScore *player_1, PlayerScore *player_2) {
	}
	// initialize the singleton pointer field
	static Resource_manager* instance;

public:
	// data structure to store all bricks
	std::vector<std::vector<Brick>> level_bricks;
	PlayerScore *display_1;
	PlayerScore *display_2;
	// Initialize sound effects
	Mix_Chunk* wall_hit_sound;
	Mix_Chunk* paddle_hit_sound;	
	// Initialize the font
	TTF_Font* score_font;


	static Resource_manager *get_instance() {
		if (!instance) instance = new Resource_manager(0,0);
		return instance;
	}

	void load_resources(SDL_Renderer* renderer) {
		// load sound resources
	    wall_hit_sound   = Mix_LoadWAV("media/wall_hit.wav");
	    paddle_hit_sound = Mix_LoadWAV("media/paddle_hit.wav");
	    // load font
	    score_font = TTF_OpenFont("media/DejaVuSansMono.ttf", 40);
	    // score display
    	display_1 = new PlayerScore(Vec2(SCREEN_WIDTH / 4, 0), renderer, score_font);
    	// notification text display
    	display_2 = new PlayerScore(Vec2( SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2), renderer, score_font);
    
	}
	void load_level(const std::string& filename) {
		// open the file
		//std::vector<std::vector<Brick>> bricks;
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
	        std::cout << std::endl;
	    }
	}

	~Resource_manager(){
		
	}

	void destroy() {
		delete display_1;
		delete display_2;
		TTF_CloseFont(score_font);
	    Mix_FreeChunk(wall_hit_sound);
	    Mix_FreeChunk(paddle_hit_sound);
		level_bricks.clear();
	}
};

#endif