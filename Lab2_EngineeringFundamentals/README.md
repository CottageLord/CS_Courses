# Lab 2 - C++ Fundamentals

> For this assignment we are going to get up and running with C++!

**Lab materials must be pushed to your repository one week from now before the next class begins**

**Read:** *Labs are designed to be finished in class if you work diligently, but expected to take 1-2 hours outside of class. They are often more 'tutorial' in style, and sometimes the solution may even be provided in pieces for you to type in and experiment. Copying & Pasting is discouraged however--make mistakes, play, and you will further learn the material.*

## Modify this section

- How many hours did it take you to complete this lab?
- Did you collaborate with any other students/TAs/Professors?
- Did you use any external resources? (Cite them below)
  - tbd
  - tbd
- (Optional) What was your favorite part of the lab?
- (Optional) How would you improve the lab?

## Logistics

You will be working on your own laptop today.

## Part 0 - Git

Make sure you completed the git tutorial so you understand how to push code to this repo! :)
[Git in 15 minutes](https://try.github.io/levels/1/challenges/1 "Try git").

Roughly speaking, here is how to use 'git' if you have not previously:

1. Run `git clone your_repository_url` to establish a git repository on your system. *your_repository_url* is found by clicking the 'green' button on you Monorepo homepage(move one directory up from this folder).

	i. It should look something like `https://github.com/CourseName/Monorepo-yourname.git`
	
2. When you make a change to a file within this directory you can see what is different by:

	i. run `git status` to see what you have changed locally on your computer. Changes you have made locally have not yet been saved to github's servers. 

3. When you are happy with your changes do `git add whatever_file.c` which prepares that specific file to be added to the master. [[git add documentation](https://git-scm.com/docs/git-add)]

4. Next, you will do `git commit -m "some informative message about your changes"` to record changes in your repository [[git-commit documentation](https://git-scm.com/docs/git-commit)]

5. Finally, do a `git push` to actually make things happen--meaning everything you have added will go to the github server. You can check your github.com repository to see exactly what I will see. [[git push documentation](https://git-scm.com/docs/git-push)]

## Part 1 - Setting up your Environment

Follow the respective tutorial for downloading and setting up SDL for your operating system. When you are finished, all you should have to do is run `python3 build.py` in your terminal, run `./lab`, and a red window should pop up.

I have provided some links below. [Lazy Foo](http://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php) also has a nice set for a variety of operating systems.

* [Linux](http://www.mshah.io/comp/Spring18/graphics/Lab/1/linux.html) (Tested on Ubuntu)
* Windows
  * Option 1: 
    * Download mysys from here: https://drive.google.com/file/d/116IIh7n8aoWXKDiQeonZjxXEkPPh3RME/view?usp=sharing
    * unzip the contents to a location on your machine.
    * From mysys64 launch the mingw64 terminal(a blue icon).
      * Note, you may have to navigate to the lab folder wherever you have cloned this repo. (e.g. cd /c/Users)
    * Try running the build script with `python build.py` and then execute lab.exe
  * Option 2: 
    * [MinGW 64](http://www.mshah.io/comp/Spring18/graphics/Lab/1/MinGW.html) (Tested on Windows 10)
      * These are some more scattered instructions on installation.
  * Option 3: (Least preferred)
      * Use visual studio here: http://www.mshah.io/comp/Spring18/graphics/Lab/1/windows.html
* [Mac](http://www.mshah.io/comp/Spring18/graphics/Lab/1/mac.html) (Tested on High Sierra 10.13.2 Operating System)
 * Something else? Let your instructor know. Typically labs and assignments are tested on all three configurations above (though in the future there will be 1 set of directions to follow after you are setup!)
* Remote graphics through a terminal (Optional--and here primarily for very limited situations)

## Part 2 - Simple DirectMedia Layer (SDL) (Reading ~2 minutes)

SDL("Simple DirectMedia Layer") is a library that allows us to draw a window to the screen. We will be using SDL version 2.0+ for this course. SDL itself is an abstraction layer that can create windows for the appropriate platform (i.e. Windows, Mac, Linux, mobile, etc.), without us have to do the real nitty gritty details of setting up a window.

Some additional notes:

- If you want to know how to setup a window in your native operating system platform the long way (without using a library like SDL), here are some examples from the web: [Linux](https://stackoverflow.com/questions/15091514/how-do-i-create-a-window-in-opengl-on-linux-instead-of-using-glut), [Linux X11](https://www.opengl.org/discussion_boards/showthread.php/177999-GCC-OpenGL-without-glut?p=1239444&viewfull=1#post1239444), [Graphics Context in Mac](https://developer.apple.com/library/content/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-CJBDCHAC), [Native Windows](http://www.falloutsoftware.com/tutorials/gl/gl2.htm)
  - (Note: It is always worth knowing how things work behind the scenes! In doing so however, you may further appreciate the work done by SDL for us!)
- We are not going to spend time arguing if other libraires (glut, freeglut, glfw, etc.) are the better or the best.
  - Try the one that fits your need
  - I choose SDL2 because it is what modern commercial games use, it is free, and it supports many desktop environments (as well as mobile if you are interested!)

# Part 3 - Coding Style

List one good coding style practice you have learned (Specify if in another language other than C++):

Frequently review the code to make it not only runnable, but readable, maintainable and expandable. In specific, this includes:
- perioducal test: I usually test from the very beginning, including file reading, main program loop, etc.
- consistent indentation & naming convention
- descriptions above each file and class / function
- meaningful in text comment
- apply modularity, reduce redundancy - high cohesion and low coupling

# Part 4 - Pong

Let me take you back in time to one of the earliest games--[pong](https://en.wikipedia.org/wiki/Pong). Pong is a table-tennis game where two paddles move up and down and send a ball back in forth. It is largely credited as the first commercially successful game. And if you can believe it, you can probably make the hit game in one sitting! This will be our first game project in the course!

## Pong Requirements

<img align="right" width="300px" src="https://media1.tenor.com/images/40fac3c55a73e15ef82705ab5d51245d/tenor.gif?itemid=16894549">

The requirements overall are loose, but we should see a working implementation of pong.

1. There should be two paddles that move
	- Using the arrowkeys to move one paddle, and WASD for the other.
	- (If you'd like to implement an A.I. for one of the paddles, you may do so)
2. A ball that has a paddle collide back and forth
3. Your game should be frame capped to 60 Frames per second and update at a fixed-time step.
4. The first player to score 3 points wins, and the game should reset.
	- A message in the console or on the screen should indicate that a player has won or lost.
5. You may otherwise get as creative as you like
	- (i.e. Sound, images, or other assets are not required)

You are free to use this excellent resource below as inspiration for building a pong clone. 

**Key Resource** https://austinmorlan.com/posts/pong_clone/

My objective for providing you this implementation is:

1. I want you working in C++ and SDL2
2. I want you building the game in an object-oriented way (which this tutorial shows some examples of).
3. You can probably find many implementations online anyway, and I think this one is pretty well explained.

## Helpful Resources

Some additional resources to help you through this lab assignment

- http://lazyfoo.net/tutorials/SDL/ 
	- This is the page where I learned how to setup SDL. 
	- Part of this lab is inspired by this websites tutorial.
- https://www.learncpp.com/
	- Useful resource for learning C++
- http://www.cplusplus.com/
	- Useful reference and tutorial for learning C++

## Deliverables

- In part 3, modify the readme and list at least one good code practice you have learned.
- In part 4, implement pong.
	- We should be able to run `python3 build.py` and execute the **lab** executable to see the working SDL application plays the game 'pong'.

You must complete this lab by next class.

## Rubric

You (and any partner(s)) will receive the same grade from a scale of 0-2.

- 0 for no work completed by the deadline
- 1 for some work completed, but something is not working properly
- 2 for a completed lab (with possible 'going further' options completed)

## Going Further

What is that, you finished Early? Did you enjoy this lab? Here are some (optional) ways to further this assignment.

- Check out some more SDL tutorials

## Found a bug?

If you found a mistake (big or small, including spelling mistakes) in this lab, kindly send me an e-mail. It is not seen as nitpicky, but appreciated! (Or rather, future generations of students will appreciate it!)

- Fun fact: The famous computer scientist Donald Knuth would pay folks one $2.56 for errors in his published works. [[source](https://en.wikipedia.org/wiki/Knuth_reward_check)]
- Unfortunately, there is no monetary reward in this course :)
