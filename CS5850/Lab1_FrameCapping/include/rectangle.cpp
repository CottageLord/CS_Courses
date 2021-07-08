#include "SDLGraphicsProgram.h"

//TODO break this into .h and .cpp

// How many blocks to draw on the screen
#define BLOCKS          500

class Rectangle{
public:
    Rectangle(){
        // Empty constructor! We have complete control!
    }

    // Okay, but do not forget to call this!
    void init(int screenWidth, int screenHeight){
        x = rand()%screenWidth;
        y = rand()%screenHeight;
        w = rand()%100;
        h = rand()%100;
        speed = rand()%2+1; 
    }

    // Arguments here are a little redundant--do we need them?
    // (Perhaps if the screen resizes? Hmm!)
    void update(int screenWidth, int screenHeight){
        if(up){
            y+=speed;
            if(y>screenHeight){
                up = !up;
            }
        }

        if(!up){
            y-=speed;
            if(y<0){
                up = !up;
            }
        }
        if(left){
            x+=speed;
            if(x>screenWidth){
                left = !left;
            }
        }

        if(!left){
            x-=speed;
            if(x<0){
                left = !left;
            }
        }
    }

    // Okay, render our rectangles!
    void render(SDL_Renderer* gRenderer){
        SDL_Rect fillRect = {x,y,w,h};
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderDrawRect(gRenderer, &fillRect); 
    }

private:
     
    int x{100};
    int y{100};
    int w{100};
    int h{100};
    int speed{1};
    bool up{true};
    bool left{true};

};


// Create our list of rectangles!
Rectangle r[BLOCKS];
