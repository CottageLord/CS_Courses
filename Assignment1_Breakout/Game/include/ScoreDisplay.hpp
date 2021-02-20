#ifndef SCORE_DISPLAY_CLASS
#define SCORE_DISPLAY_CLASS

#include "config.hpp"
#include "PlayerScore.hpp"
// This is for holding score display pointers
class ScoreDisplay {
public:
	PlayerScore *display_1;
	PlayerScore *display_2;
	ScoreDisplay(PlayerScore *player_1, PlayerScore *player_2) {

	}
	~ScoreDisplay(){
		delete display_1;
		delete display_2;
	}
};

#endif

