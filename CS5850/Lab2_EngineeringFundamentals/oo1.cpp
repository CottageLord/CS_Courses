/*

Compile the following with one of the following
  - clang++ -std=c++14 -o oop oo1.cpp
  - g++ -std=c++14 -o oop oo1.cpp 
  - Visual Studio: Compile & run as normal, though you may need to
		   enable c++14 switch.
                   | https://docs.microsoft.com/en-gb/cpp/build/reference/std-specify-language-standard-version
  - XCode: Running C++14 | https://medium.com/@Rpessoa/how-to-code-in-c-14-with-xcode-8-2cb2eca9e5c8

(Optional Read:) In case you wonder why C++14, here are some features we get:
  - https://isocpp.org/wiki/faq/cpp14-language
  - https://isocpp.org/wiki/faq/cpp11-language
*/

#include <iostream>

// Everything in our game is a node
class node{
public:

	node(){
		std::cout << "Node constructor called\n";
	}

	~node(){
		std::cout << "Node destructor called\n";	
	}

private:
	int id;

};


// A Vector3 inherits from a node
class Vector3 : public node{
public:
	float x,y,z;

	Vector3(float x, float y, float z): x(x),y(y),z(z) {
		std::cout << " Vector3 constructor called\n";
	}

	~Vector3(){
		std::cout << " Vector3 destructor called\n";	
	}

};


// A bullet has x,y,z.
class Bullet: public Vector3{
public:
	Bullet(float x, float y, float z): Vector3(x,y,z){
		std::cout << "  Bullet constructor called\n";
	} 

	~Bullet(){	
		std::cout << "  Bullet destructor called\n";
	}

};


class LaserRocket : public Bullet{
public:

	Bullet* bullets;

	LaserRocket(float x, float y, float z): Bullet(x,y,z){
		std::cout << "   LaserRocket constructor called\n";
		std::cout << "   Coordinates are: " << x << "," << y << "," << z << "\n";
	}

	~LaserRocket(){
		std::cout << "   LaserRocket destructor called\n";
	}

	void shootLasers(){
		/*  Laser shooting loop here with bullets */
	}
	

};



/* ******************************************** 

********************************************* */
int main(){

	LaserRocket myLaserRocket(1.0f,2.0f,3.0f);	

	return 0;
}
