/*!\mainpage  Breakout
* <table>
* <tr><th>Project  <td>Breakout
* <tr><th>Author   <td>Yang Hu
* </table>
* \section Project description
* -# A simple SDL breakout game built on Windows 10.
* -# SDL2, SDL_image, SDL_ttf and SDL_mixer are required for running this game.
*
* \section Naming convention
* Files with first letter capitalized are class files.
*
* \section Play Instructions
* -# Use A/D on the key board to control your paddle.
* -# Use L to launch the ball when the ball stays on the paddle
* -# Use R to start the game when the game end.
* -# Use Q to quit the game.
*
**********************************************************************************
*/

/** @file lab.cpp
@author yang hu
@version 1.0
@brief implements lab.hpp
@date Monday, Feburuary 22, 2021
*/

#include "lab.hpp"

Resource_manager* Resource_manager::instance = nullptr;

// Initialization function
bool init_window() {
    // Initialization flag
    bool success = true;
    // String to hold any errors that occur.
    std::stringstream errorStream;
    // The window we'll be rendering to
    g_window = NULL;
    // Render flag
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO)< 0){
        errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
        success = false;
    }
    else{
        //Create window
        g_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH,
                                SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN );
        // Check if Window did not create.
        if( g_window == NULL ){
            errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }
        //Create a Renderer to draw on
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        // Check if Renderer did not create.
        if( g_renderer == NULL ){
            errorStream << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }
    }

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "Failed to initialize!\n";
        std::string errors=errorStream.str();
        std::cout << errors << "\n";
    } else {
        std::cout << "No SDL, or OpenGL, errors Detected\n\n";
    }
    return success;
}

bool init_components() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    TTF_Init();
    Resource_manager::get_instance()->load_resources(g_renderer);
    Resource_manager::get_instance()->load_level(LEVEL_FILES[current_level]);
    Mix_PlayMusic(Resource_manager::get_instance()->background_music, -1);

    //Resource_manager::get_instance()->load_text(language_file);
    return true;
}

bool game_running() {
    return language_selected;
}

void language_selection() {
    Resource_manager::get_instance()->display_2->set_text(
        "    English[E]     Français[F]");
    Resource_manager::get_instance()->display_2->Draw();
    SDL_RenderPresent(g_renderer);

}
void restart_game() {
    // reload level
    current_level = 0;
    Resource_manager::get_instance()->level_bricks.clear();
    Resource_manager::get_instance()->load_level(LEVEL_FILES[current_level]);
    // restore lives
    player_life = PLAYER_LIFE;
    /*
    player_1_score = 0;
    player_2_score = 0;*/
}

// This is where we do work in our graphics applications
// that is constantly refreshed.
void update(double elapsed_time) {
    // ================= Update the paddle ================//

    paddle_1.Update(elapsed_time);
    
    // ================= Update the ball position ================//
    Contact contact;

    if (ball_with_paddle)
    {
        ball.position.x = paddle_1.position.x + paddle_1.rect.w / 2 - ball.rect.w / 2;
        ball.position.y = paddle_1.position.y - ball.rect.h;
        //ball.velocity = Vec2(0.0f, BALL_SPEED);
    } else {
        // =================== Check collisions ==================//
        // if collides, update velocity
        if (contact = check_obj_collision(ball, paddle_1);
        contact.type != Collision_type::None)
        {
            //std::cout<< (int)contact.type << " " << (int)contact.side << std::endl;
            ball.collide_with_paddle(contact);
            Mix_PlayChannel(-1, Resource_manager::get_instance()->paddle_hit_sound, 0);
        }
        // wall collision
        else if (contact = check_wall_collision(ball);
            contact.type != Collision_type::None)
        {
            ball.collide_with_wall(contact);
            Mix_PlayChannel(-1, Resource_manager::get_instance()->wall_hit_sound, 0);
        }

        // draw all bricks
        for (int i = Resource_manager::get_instance()->level_bricks.size() - 1; i >= 0; i--)
        {
            for (int j = 0; j < Resource_manager::get_instance()->level_bricks[0].size(); j++)
            {
                if (Resource_manager::get_instance()->level_bricks[i][j].status != Brick_type::None)
                {
                    if (contact = check_obj_collision(ball, Resource_manager::get_instance()->level_bricks[i][j]);
                    contact.type != Collision_type::None)
                    {
                        // perform collision
                        ball.collide_with_brick(contact);
                        // update brick manager
                        Resource_manager::get_instance()->level_bricks[i][j].status = Brick_type::None;
                        bricks_remained--;
                        // play sound
                        Mix_PlayChannel(-1, Resource_manager::get_instance()->paddle_hit_sound, 0);
                    }
                }
            }
        }
        ball.Update(elapsed_time);
    }

    // ================= Update the game message ================//
    
    // the score/life txt that will be constantly updated on screen
    std::string game_stat = Resource_manager::get_instance()->text_display[(int)Text_order::Lifes] + 
        std::to_string(player_life) + 
        Resource_manager::get_instance()->text_display[(int)Text_order::Bricks] + 
        std::to_string(bricks_remained);

    Resource_manager::get_instance()->display_1->set_text(game_stat);
    // =================== check win condition ==================//
    if (bricks_remained <= 0){
        // if level remaining
        if (current_level < LEVEL_NUM)
        {
            if(!pause) current_level++;
            Resource_manager::get_instance()->display_2->set_text(
                Resource_manager::get_instance()->text_display[(int)Text_order::Win_next]);
            ball_with_paddle = true;
            pause = true;
        } else {
            Resource_manager::get_instance()->display_2->set_text(
                Resource_manager::get_instance()->text_display[(int)Text_order::Win_end]);
            ball_with_paddle = true;
            pause = true;
        }
        

    } else if (player_life <= 0) {
        Resource_manager::get_instance()->display_2->set_text(
            Resource_manager::get_instance()->text_display[(int)Text_order::Lose_end]);
        ball_with_paddle = true;
        pause = true;

    } else if(ball_with_paddle) {
        Resource_manager::get_instance()->display_2->set_text(
            Resource_manager::get_instance()->text_display[(int)Text_order::Launch_ball]);
    } else Resource_manager::get_instance()->display_2->set_text("");
    /*
    if (player_1_score >= SCORE_TO_WIN || player_2_score >= SCORE_TO_WIN)
    {
        player_1_win = player_1_score >= SCORE_TO_WIN;
        restart_game();
    }*/
}

// This function draws images.
void render() {
    //============= Clear the window to dark red ==============//
    SDL_SetRenderDrawColor(g_renderer, 0x55, 0x0, 0x0, 0xFF);
    SDL_RenderClear(g_renderer);

    //==================== Draw the bricks ====================//

    // Set the draw color to be white
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    
    //==================== Draw the ball ==================//

    ball.Draw(g_renderer);
    
    //=================== Draw the paddle =================//

    paddle_1.Draw(g_renderer);
    //paddle_2.Draw(g_renderer);

    //=================== Draw the score ==================//


    Resource_manager::get_instance()->display_1->Draw();
    Resource_manager::get_instance()->display_2->Draw();

    //================= Render all elements ================//
    // Set the draw color to be blue
    SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x3F, 0xFF);
    // draw all bricks
    for (int i = Resource_manager::get_instance()->level_bricks.size() - 1; i >= 0; i--)
    {
        for (int j = 0; j < Resource_manager::get_instance()->level_bricks[0].size(); j++)
        {
            //std::cout << (int)((level_bricks[i][j]).status);
            if (Resource_manager::get_instance()->level_bricks[i][j].status != Brick_type::None)
            {
                Resource_manager::get_instance()->level_bricks[i][j].Draw(g_renderer);
            }
        }
    }

    SDL_RenderPresent(g_renderer);
}

// Proper shutdown and destroy initialized objects
void close() {

    // close the font
    //TTF_CloseFont(score_font);
    Resource_manager::get_instance()->destroy();

    Mix_Quit();
    TTF_Quit();

    //delete &resource_manager;
    // Destroy Renderer
    SDL_DestroyRenderer(g_renderer);
    //Destroy window
    SDL_DestroyWindow( g_window );

    // Point g_window to NULL to ensure it points to nothing.
    g_renderer = NULL;
    g_window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}
// the Update() helper function that provide a frame stablizer
// adapted from my lab 1
void update_with_timer(std::chrono::steady_clock::time_point &previous_time, 
    double &elapsed_time_total, int &frame_counter, double &lag, double &mcs_per_update) {
    // time recorders
    //std::cout << "prev time: " << std::chrono::duration_cast<std::chrono::minutes>(previous_time).count();
    std::chrono::steady_clock::time_point current_time;
    // calculate how much time has elapsed since last record (usually 1 render loop earlier)
    current_time  = std::chrono::steady_clock::now();

    double elapsed_time;
    elapsed_time  = (double)std::chrono::duration_cast<std::chrono::microseconds>
                      (current_time - previous_time).count();
    // renew time record
    previous_time = current_time;
    // record overall time spend (in microseconds)
    elapsed_time_total += elapsed_time;
    //std::cout << "+ " << elapsed_time << " = " << elapsed_time_total << std::endl;
    // stablizer switch
    if(stable_frame) {
        // if the last update/render loop spent more than fps limit (16.67ms for 60 fps)
        // we update untill the game progress catches up
        lag += elapsed_time;
        while(lag >= mcs_per_update) {
            // Update our scene
            update(lag);
            lag -= mcs_per_update;
            frame_counter++;
        }
    } else {
        update(lag);
        frame_counter++;
    }
    // for every 1 second, report frame rate and re-initialize counters
    if (elapsed_time_total > mcs_per_second)
    {
        std::cout << "current frame rate: " << frame_counter << std::endl;
        frame_counter = 0;
        elapsed_time_total = 0.0;
    }
}
// handle various keyboard event
void handle_event(bool &quit) {
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event event;
    //Handle events on queue
    while(SDL_PollEvent( &event ) != 0){
        // User posts an event to quit
        // An example is hitting the "x" in the corner of the window.
        if(event.type == SDL_QUIT){
            quit = true;
        } else if (event.type == SDL_KEYDOWN) { // respond to various keyboard inputs
            if (event.key.keysym.sym == SDLK_ESCAPE)    quit = true;
            // restart game available when win/lose last one
            if (event.key.keysym.sym == SDLK_r && pause == true){
                pause = false;
                if(player_life > 0) {
                    Resource_manager::get_instance()->level_bricks.clear();
                    Resource_manager::get_instance()->load_level(LEVEL_FILES[current_level]);
                } else restart_game();
            }       
            else if (event.key.keysym.sym == SDLK_a)    buttons[(int)Buttons::paddle_1_left]  = true;
            else if (event.key.keysym.sym == SDLK_d)    buttons[(int)Buttons::paddle_1_right] = true;
            // if language not selected
            else if (event.key.keysym.sym == SDLK_e && !language_selected) {
                language_file = ENGLISH_FILE; 
                Resource_manager::get_instance()->load_text(language_file);
                language_selected = true;
            }
            else if (event.key.keysym.sym == SDLK_f && !language_selected) {
                language_file = FRENCH_FILE;
                Resource_manager::get_instance()->load_text(language_file);
                language_selected = true;
            }
            //else if (event.key.keysym.sym == SDLK_w)    buttons[(int)Buttons::paddle_1_up]    = true;
            //else if (event.key.keysym.sym == SDLK_s)    buttons[(int)Buttons::paddle_1_down]  = true;
            else if (event.key.keysym.sym == SDLK_l)    ball_with_paddle = false;
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_a)         buttons[(int)Buttons::paddle_1_left]  = false;
            else if (event.key.keysym.sym == SDLK_d)    buttons[(int)Buttons::paddle_1_right] = false;
            //else if (event.key.keysym.sym == SDLK_w)    buttons[(int)Buttons::paddle_1_up]    = false;
            //else if (event.key.keysym.sym == SDLK_s)    buttons[(int)Buttons::paddle_1_down]  = false;
        }
    }

    // adjust paddle's velocity accordingly
    if (buttons[(int)Buttons::paddle_1_left])        paddle_1.velocity.x = -PADDLE_SPEED;
    else if (buttons[(int)Buttons::paddle_1_right])  paddle_1.velocity.x = PADDLE_SPEED;
    //else if (buttons[(int)Buttons::paddle_1_up])     paddle_1.velocity.y = -PADDLE_SPEED;
    //else if (buttons[(int)Buttons::paddle_1_down])   paddle_1.velocity.y = PADDLE_SPEED;
    else
    {                      
        paddle_1.velocity.x = 0.0f;
        paddle_1.velocity.y = 0.0f;
    }
}

// execute main game loop
void loop() {
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // define microseconds per update
    double mcs_per_update = mcs_per_second / frame_rate;
    // elapsed_time_total - measure total time elapsed
    // lag - accumulate elapsed time for determining when to update to ensure steady frame rate
    double lag = 0, elapsed_time_total = 0.0;
    // frame_counter - measure the real frame rate
    int frame_counter = 0;

    // ==================== LOGIC BEGIN ==================== //

    // Enable text input
    SDL_StartTextInput();

    // chrono::steadyclock for measuring times accurately while rendering
    std::chrono::steady_clock::time_point previous_time;
    // record the initial time
    previous_time = std::chrono::steady_clock::now();
    // While application is running

    while(!quit){
        handle_event(quit);
        if (game_running())
        {
            // update with a frame stablizer
            update_with_timer(previous_time, elapsed_time_total, frame_counter, lag, mcs_per_update);
            // Render using SDL
            render();
        } else language_selection();
        
    } 

    //Disable text input
    SDL_StopTextInput();
}
// Our entry point into our program.
// Remember, C++ programs have exactly one entry point
// where the program starts.
int main(int argc, char* args[])
{
    // Start up SDL and create window
    
    if (init_window() && init_components()) loop();
    close();
    
    return 0;
}
