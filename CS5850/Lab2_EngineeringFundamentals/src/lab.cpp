// Support Code written by Michael D. Shah
// This is modified from a tutorial from: http://lazyfoo.net/tutorials/SDL/51_SDL_and_modern_opengl/index.php
//
// You can modify this could however you like in order to implement your 'pong'
// By default this code just outputs a simple 'red' screen.
//
// If this code does NOT work, try the source from this online example: http://lazyfoo.net/tutorials/SDL/50_SDL_and_opengl_2/index.php
// (Please alert the instructor if this is the case)
//
//
// Last Updated: 2/1/21
// Please do not redistribute without asking permission.

#include "lab.hpp"
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
    }else{
        std::cout << "No SDL, or OpenGL, errors Detected\n\n";
    }
    return success;
}

bool init_components() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    // load font
    score_font = TTF_OpenFont("DejaVuSansMono.ttf", 40);
    // load score display
    score_display.display_1 = new PlayerScore(Vec2(SCREEN_WIDTH / 4, 20), g_renderer, score_font);
    // player 2 at 3/4 width pos
    score_display.display_2 = new PlayerScore(Vec2(3 * SCREEN_WIDTH / 4, 20), g_renderer, score_font);
    

    return true;
}

void restart_game() {
    pause = true;
    player_1_score = 0;
    player_2_score = 0;
}

// This is where we do work in our graphics applications
// that is constantly refreshed.
void update(double elapsed_time) {

    // ================= Update the player score ================//
    // if already win, show win msg
    if(pause){
        if (player_1_win)
        {
            score_display.display_1->set_score("P1 wins!     press R");
            score_display.display_2->set_score("to restart!");
        }
        else if (!player_1_win) {
            score_display.display_1->set_score("P2 wins!     press R");
            score_display.display_2->set_score("to restart!");
        }
        // reset the ball / paddle positions
        ball.position.x = (SCREEN_WIDTH / 2.0f) - (BALL_WIDTH / 2.0f);
        ball.position.y = (SCREEN_HEIGHT / 2.0f) - (BALL_WIDTH / 2.0f);

        paddle_1.position.x = 50.0f;
        paddle_1.position.y = (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f);

        paddle_2.position.x = SCREEN_WIDTH - 50.0f;
        paddle_2.position.y = (SCREEN_HEIGHT / 2.0f) - (PADDLE_HEIGHT / 2.0f);

        // else keep the game run
    } else {
        // Update the paddle positions
        paddle_1.Update(elapsed_time);
        paddle_2.Update(elapsed_time);

        Contact contact;
        // =================== Check collisions ==================//
        // if collides, update velocity
        if (contact = check_paddle_collision(ball, paddle_1);
        contact.type != CollisionType::None)
        {
            ball.collide_with_paddle(contact);
        }
        else if (contact = check_paddle_collision(ball, paddle_2);
            contact.type != CollisionType::None)
        {
            ball.collide_with_paddle(contact);
        }
        // wall collision
        else if (contact = check_wall_collision(ball);
            contact.type != CollisionType::None)
        {
            ball.collide_with_wall(contact);
        }
        // ================= Update the ball position ================//

        ball.Update(elapsed_time);

        // ================= Update the score ================//

        if (contact.type == CollisionType::Left) ++player_2_score;
        else if (contact.type == CollisionType::Right) ++player_1_score;

        score_display.display_1->set_score(std::to_string(player_1_score));
        score_display.display_2->set_score(std::to_string(player_2_score));
    }

    // =================== check win condition ==================//

    if (player_1_score >= SCORE_TO_WIN || player_2_score >= SCORE_TO_WIN)
    {
        player_1_win = player_1_score >= SCORE_TO_WIN;
        restart_game();
    }
}

// This function draws images.
void render() {
    //============= Clear the window to dark red ==============//
    SDL_SetRenderDrawColor(g_renderer, 0x55, 0x0, 0x0, 0xFF);
    SDL_RenderClear(g_renderer);

    //==================== Draw the net ====================//

    // Set the draw color to be white
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    // Draw the net
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        if (y % 5)
        {
            SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH / 2, y);
        }
    }

    //==================== Draw the ball ==================//

    ball.Draw(g_renderer);
    
    //=================== Draw the paddle =================//

    paddle_1.Draw(g_renderer);
    paddle_2.Draw(g_renderer);

    //=================== Draw the score ==================//


    score_display.display_1->Draw();
    score_display.display_2->Draw();

    //================= Render all elements ================//

    SDL_RenderPresent(g_renderer);
}

// Proper shutdown and destroy initialized objects
void close() {

    // close the font
    //TTF_CloseFont(score_font);
    TTF_Quit();

    delete &score_display;
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
void update_with_timer(std::chrono::steady_clock::time_point &previous_time, double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update) {
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
    // stablizer switch
    if(stable_frame) {
        // if the last update/render loop spent more than fps limit (16.67ms for 60 fps)
        // we update untill the game progress catches up
        lag += elapsed_time;
        while(lag >= mcs_per_update) {
            // Update our scene
            // cast the time down to make the number easy to calculate
            update(lag / time_cast); // should be "lag" :(
            lag -= mcs_per_update;
            frame_counter++;
        }
    } else {
        update(lag / time_cast);
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
            if (event.key.keysym.sym == SDLK_r)         pause = false;
            else if (event.key.keysym.sym == SDLK_w)    buttons[Buttons::paddle_1_up]   = true;
            else if (event.key.keysym.sym == SDLK_s)    buttons[Buttons::paddle_1_down] = true;
            else if (event.key.keysym.sym == SDLK_UP)   buttons[Buttons::paddle_2_up]   = true;
            else if (event.key.keysym.sym == SDLK_DOWN) buttons[Buttons::paddle_2_down] = true;
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_w)         buttons[Buttons::paddle_1_up]   = false;
            else if (event.key.keysym.sym == SDLK_s)    buttons[Buttons::paddle_1_down] = false;
            else if (event.key.keysym.sym == SDLK_UP)   buttons[Buttons::paddle_2_up]   = false;
            else if (event.key.keysym.sym == SDLK_DOWN) buttons[Buttons::paddle_2_down] = false;
        }
    }

    // adjust paddle's velocity accordingly
    if (buttons[Buttons::paddle_1_up])        paddle_1.velocity.y = -PADDLE_SPEED;
    else if (buttons[Buttons::paddle_1_down]) paddle_1.velocity.y = PADDLE_SPEED;
    else                                      paddle_1.velocity.y = 0.0f;

    if (buttons[Buttons::paddle_2_up])        paddle_2.velocity.y = -PADDLE_SPEED;
    else if (buttons[Buttons::paddle_2_down]) paddle_2.velocity.y = PADDLE_SPEED;
    else                                      paddle_2.velocity.y = 0.0f;
}

// execute main game loop
void loop() {
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // define microseconds per update
    double mcs_per_update = mcs_per_second / frame_rate;
    double ms_per_update = mcs_per_update / 1000;
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
        // update with a frame stablizer
        update_with_timer(previous_time, elapsed_time_total, frame_counter, lag, mcs_per_update);
        // Render using SDL
        render();
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
