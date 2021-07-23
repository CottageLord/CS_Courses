# Run with:
#
# (Linux & Mac) python3.5 test.py -m ./mygameengine.so
# (Windows) python3.6 test.py -m ./mygameengine.pyd
#
# The program should also run with 'python2.7' but you will have
# to change the 3.5's to 2.7's in your respective build script as
# well as make sure you compiled with 3.5 or 2.7 load flags.
#
# You will see `python3.5-config --includes` for example which corresponds
# to which version of python you are building.
# (In fact, run `python3.5-config --includes` in the terminal to see what it does!)
import mygameengine
import time
# Now use some python libraries for random numbers!
import random

pause = False

def update_game(game, elapsed_time, ball, paddle_1, paddle_2):
    if(pause):
        # reset the ball / paddle positions
        ball.position.x = (mygameengine.SCREEN_WIDTH / 2.0) - (mygameengine.BALL_WIDTH / 2.0)
        ball.position.y = (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.BALL_WIDTH / 2.0)

        paddle_1.position.x = 50.0
        paddle_1.position.y = (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.PADDLE_HEIGHT / 2.0)

        paddle_2.position.x = mygameengine.SCREEN_WIDTH - 50.0
        paddle_2.position.y = (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.PADDLE_HEIGHT / 2.0)
        # else keep the game run
    else:
        # Update the paddle positions
        paddle_1.Update(elapsed_time);
        paddle_2.Update(elapsed_time);

        # =================== Check collisions ==================//
        # if collides, update velocity
        game.resolve_collision(ball, paddle_1, paddle_2)
        game.response_to_input(paddle_1, paddle_2)
        # ================= Update the ball position ================//
        ball.Update(elapsed_time)

def render_game(ball, paddle_1, paddle_2):
    ball.Draw()
    paddle_1.Draw()
    paddle_2.Draw()
    
def main():
    # Initialize SDL
    game = mygameengine.SDLGraphicsProgram(mygameengine.SCREEN_WIDTH,mygameengine.SCREEN_HEIGHT)

    # Our main game loop
    # Note: This is a simple frame stablizer

    frame_rate = 60
    frame_counter = 0
    time_per_frame = 1 / frame_rate
    time_cast = 1500
    since_last_update = 0
    prev_time = time.perf_counter()
    curr_time = prev_time
    
    # record key pressed
    buttons = [False, False, False, False]
    game_playing = True

    game.StartTextInput()

    # Create the ball at the center of the screen
    ball = mygameengine.Ball(mygameengine.Vec2((mygameengine.SCREEN_WIDTH / 2.0) - (mygameengine.BALL_WIDTH / 2.0),
        (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.BALL_WIDTH / 2.0)),
        mygameengine.Vec2(mygameengine.BALL_SPEED, 0.0))

    # Create the paddles
    paddle_1 = mygameengine.Paddle(mygameengine.Vec2(50.0, 
        (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.PADDLE_HEIGHT / 2.0)),
        mygameengine.Vec2(0.0, 0.0))
    
    paddle_2 = mygameengine.Paddle(mygameengine.Vec2(mygameengine.SCREEN_WIDTH - 50.0, 
        (mygameengine.SCREEN_HEIGHT / 2.0) - (mygameengine.PADDLE_HEIGHT / 2.0)),
        mygameengine.Vec2(0.0, 0.0))


    while (game_playing):
        # handle SDL keyBoard event, ESCAPE to quit
        game_playing = game.handle_event()
        # measure time elapsed for frame capping
        time_diff = curr_time - prev_time
        prev_time = curr_time

        since_last_update += time_diff
        while (since_last_update >= time_per_frame):
            update_game(game, since_last_update * 250, ball, paddle_1, paddle_2)

            #game.update(since_last_update * 250)
            since_last_update -= time_per_frame
            frame_counter += 1
            #print(mygameengine.paddle_1_up)
            
        #game.delay(100);
        game.render()
        render_game(ball, paddle_1, paddle_2)
        game.flip()
        curr_time = time.perf_counter()

        if (frame_counter >= frame_rate):
            frame_counter = 0
            print("60 frames")
        
    game.StopTextInput()

main()
