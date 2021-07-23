#ifndef SDLGRAPHICSPROGRAM
#define SDLGRAPHICSPROGRAM

#include "SDLGraphicsProgram.hpp"


// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):screenWidth(w),screenHeight(h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	gWindow = NULL;
	// Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
	    //Create window
    	gWindow = SDL_CreateWindow( "Lab", 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN );

        // Check if Window did not create.
        if( gWindow == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create a Renderer to draw on
        g_renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
        // Check if Renderer did not create.
        if( g_renderer == NULL){
            errorStream << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
            success = false;
        }
  	}

    
    
    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    //Destroy window
	SDL_DestroyWindow( gWindow );
	// Point gWindow to NULL to ensure it points to nothing.
	gWindow = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::initGL(){
	//Success flag
	bool success = true;

	return success;
}



//Loops forever!
void SDLGraphicsProgram::loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // While application is running
    while(!quit){

    }

    //Disable text input
    SDL_StopTextInput();
}


// This is where we do work in our graphics applications
// that is constantly refreshed.
void SDLGraphicsProgram::update(double elapsed_time) {
    // ================= Update the player score ================//
    // if already win, show win msg
    if(pause){
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

        resolve_collision(ball, paddle_1, paddle_2);

        response_to_input(paddle_1, paddle_2);
        
        // ================= Update the ball position ================//
        ball.Update(elapsed_time);
    }
}
void SDLGraphicsProgram::response_to_input(Paddle &paddle_1, Paddle &paddle_2) {
    if (paddle_1_up)        paddle_1.velocity.y = -PADDLE_SPEED;
    else if (paddle_1_down) paddle_1.velocity.y = PADDLE_SPEED;
    else                    paddle_1.velocity.y = 0.0f;

    if (paddle_2_up)        paddle_2.velocity.y = -PADDLE_SPEED;
    else if (paddle_2_down) paddle_2.velocity.y = PADDLE_SPEED;
    else                    paddle_2.velocity.y = 0.0f;
}
// This function draws images.
void SDLGraphicsProgram::render() {
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
    /*
    //==================== Draw the ball ==================//
    ball.Draw();
    //=================== Draw the paddle =================//
    paddle_1.Draw();
    paddle_2.Draw();*/
    //=================== Draw the score ==================//
    
}
// Flip
// The flip function gets called once per loop
// It swaps out the previvous frame in a double-buffering system
void SDLGraphicsProgram::flip(){
    // Nothing yet!
    SDL_RenderPresent(g_renderer);
}

void SDLGraphicsProgram::delay(int milliseconds){
    SDL_Delay(milliseconds); 
}

// handle various keyboard event
bool SDLGraphicsProgram::handle_event() {
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event event;
    //Handle events on queue
    while(SDL_PollEvent( &event ) != 0){
        // User posts an event to quit
        // An example is hitting the "x" in the corner of the window.
        if(event.type == SDL_QUIT){
            return false;
        } else if (event.type == SDL_KEYDOWN) { // respond to various keyboard inputs
            if (event.key.keysym.sym == SDLK_ESCAPE)    return false;
            if (event.key.keysym.sym == SDLK_r)         return true;
            else if (event.key.keysym.sym == SDLK_w)    paddle_1_up   = true;
            else if (event.key.keysym.sym == SDLK_s)    paddle_1_down = true;
            else if (event.key.keysym.sym == SDLK_UP)   paddle_2_up   = true;
            else if (event.key.keysym.sym == SDLK_DOWN) paddle_2_down = true;
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_w)         paddle_1_up   = false;
            else if (event.key.keysym.sym == SDLK_s)    paddle_1_down = false;
            else if (event.key.keysym.sym == SDLK_UP)   paddle_2_up   = false;
            else if (event.key.keysym.sym == SDLK_DOWN) paddle_2_down = false;
        }
    }

    // adjust paddle's velocity accordingly
    /*
    
    */
    return true;
}
// the Update() helper function that provide a frame stablizer
// adapted from my lab 1
void SDLGraphicsProgram::update_with_timer(std::chrono::steady_clock::time_point &previous_time, double &elapsed_time_total, int &frame_counter, double &lag, double mcs_per_update) {
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

void SDLGraphicsProgram::resolve_collision(Ball &ball, Paddle &paddle_1, Paddle &paddle_2) {
    Contact contact = {CollisionType::None, 0.0f};
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
}

// check if the ball hit the paddle
Contact SDLGraphicsProgram::check_paddle_collision(Ball const& ball, Paddle const& paddle)
{
    float ball_left     = ball.position.x;
    float ball_right    = ball.position.x + ball.rect.w;
    float ball_top      = ball.position.y;
    float ball_bottom   = ball.position.y + ball.rect.h;

    float paddle_left   = paddle.position.x;
    float paddle_right  = paddle.position.x + paddle.rect.w;
    float paddle_top    = paddle.position.y;
    float paddle_bottom = paddle.position.y + paddle.rect.h;

    Contact contact{};

    // if collision occurs (a simple Separating Axis Theorem system)
    if (ball_left   >= paddle_right)    return contact;
    if (ball_right  <= paddle_left)     return contact;
    if (ball_top    >= paddle_bottom)   return contact;
    if (ball_bottom <= paddle_top)      return contact;

    // ball launches with diff speed when hitting diff paddle speed

    float paddle_range_upper  = paddle_bottom - (2.0f * paddle.rect.h / 3.0f);
    float paddle_range_middle = paddle_bottom - (paddle.rect.h / 3.0f);

    if (ball.velocity.x < 0)
    {
        // Left paddle
        contact.penetration = paddle_right - ball_left;
    }
    else if (ball.velocity.x > 0)
    {
        // Right paddle
        contact.penetration = paddle_left - ball_right;
    }

    if ((ball_bottom > paddle_top)
        && (ball_bottom < paddle_range_upper))
    {
        contact.type = CollisionType::Top;
    }
    else if ((ball_bottom > paddle_range_upper)
         && (ball_bottom < paddle_range_middle))
    {
        contact.type = CollisionType::Middle;
    }
    else
    {
        contact.type = CollisionType::Bottom;
    }

    return contact;
}

// check if the ball hit the wall
Contact SDLGraphicsProgram::check_wall_collision(Ball const& ball) {

    float ball_left     = ball.position.x;
    float ball_right    = ball.position.x + ball.rect.w;
    float ball_top      = ball.position.y;
    float ball_bottom   = ball.position.y + ball.rect.h;

    Contact contact{};

    if (ball_left < 0.0f)
    {
        contact.type = CollisionType::Left;
    }
    else if (ball_right > SCREEN_WIDTH)
    {
        contact.type = CollisionType::Right;
    }
    else if (ball_top < 0.0f)
    {
        contact.type = CollisionType::Top;
        contact.penetration = -ball_top;
    }
    else if (ball_bottom > SCREEN_HEIGHT)
    {
        contact.type = CollisionType::Bottom;
        contact.penetration = SCREEN_HEIGHT - ball_bottom;
    }

    return contact;
}

void SDLGraphicsProgram::StartTextInput() {
    SDL_StartTextInput();
}
void SDLGraphicsProgram::StopTextInput() {
    SDL_StopTextInput();
}
// Include the pybindings
#include <pybind11/pybind11.h>

namespace py = pybind11;


// Creates a macro function that will be called
// whenever the module is imported into python
// 'mygameengine' is what we 'import' into python.
// 'm' is the interface (creates a py::module object)
//      for which the bindings are created.
//  The magic here is in 'template metaprogramming'
PYBIND11_MODULE(mygameengine, m){
    m.doc() = "our game engine as a library"; // Optional docstring

    py::class_<SDLGraphicsProgram>(m, "SDLGraphicsProgram")
            .def(py::init<int,int>(), py::arg("w"), py::arg("h"))   // our constructor
            .def("render", &SDLGraphicsProgram::render)
            .def("flip", &SDLGraphicsProgram::flip)
            .def("update", &SDLGraphicsProgram::update)
            .def("delay", &SDLGraphicsProgram::delay)
            .def("handle_event", &SDLGraphicsProgram::handle_event)
            .def("check_paddle_collision", &SDLGraphicsProgram::check_paddle_collision)
            .def("check_wall_collision", &SDLGraphicsProgram::check_wall_collision)
            .def("resolve_collision", &SDLGraphicsProgram::resolve_collision)
            .def("response_to_input", &SDLGraphicsProgram::response_to_input)
            .def("StartTextInput", &SDLGraphicsProgram::StartTextInput)
            .def("StopTextInput", &SDLGraphicsProgram::StopTextInput);
            


    py::class_<Vec2>(m, "Vec2")
        .def(py::init<float, float>(), py::arg("x"), py::arg("y"));

    py::class_<Ball>(m, "Ball")
        .def(py::init<Vec2, Vec2>(), py::arg("position"), py::arg("velocity"))
        .def("Update", &Ball::Update)
        .def("collide_with_paddle", &Ball::collide_with_paddle)
        .def("collide_with_wall", &Ball::collide_with_wall)
        .def("Draw", &Ball::Draw);

    py::class_<Paddle>(m, "Paddle")
        .def(py::init<Vec2, Vec2>(), py::arg("position"), py::arg("velocity"))
        .def("Update", &Paddle::Update)
        .def("Draw", &Paddle::Draw);

    m.attr("SCREEN_WIDTH") = SCREEN_WIDTH;
    m.attr("SCREEN_HEIGHT") = SCREEN_HEIGHT;
    m.attr("BALL_WIDTH") = BALL_WIDTH;
    m.attr("PADDLE_HEIGHT") = PADDLE_HEIGHT;
    m.attr("PADDLE_SPEED") = PADDLE_SPEED;
    m.attr("BALL_SPEED") = BALL_SPEED;
    
    m.attr("paddle_1_up") = paddle_1_up;
    m.attr("paddle_1_down") = paddle_1_down;
    m.attr("paddle_2_up") = paddle_2_up;
    m.attr("paddle_2_down") = paddle_2_down;

    py::enum_<CollisionType>(m, "CollisionType", py::arithmetic())
    .value("None", CollisionType::None)
    .value("Top", CollisionType::Top)
    .value("Middle", CollisionType::Middle)
    .value("Bottom", CollisionType::Bottom)
    .value("Left", CollisionType::Left)
    .value("Right", CollisionType::Right)
    .export_values();

    //py::class_<>()

// We do not need to expose everything to our users!
//            .def("getSDLWindow", &SDLGraphicsProgram::getSDLWindow, py::return_value_policy::reference) 
}
#endif
