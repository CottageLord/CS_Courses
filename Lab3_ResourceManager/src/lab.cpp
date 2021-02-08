// Support Code written by Michael D. Shah
// Last Updated: 2/07/21
// Please do not redistribute without asking permission.

#include "SDLGraphicsProgram.hpp"
#include "ResourceManager.hpp"

int main(int argc, char** argv){
	// Create an instance of an object for a SDLGraphicsProgram
	SDLGraphicsProgram mySDLGraphicsProgram(1280,720);
	// Run our program forever
	mySDLGraphicsProgram.loop();
	// When our program ends, it will exit scope, the
	// destructor will then be called and clean up the program.
	return 0;
}
