// g++ -std=c++14 3_shared_ptr.cpp -o 3_shared_ptr
// 
// An alternative of the previous program.
// 
#include <memory> // Need to include this library for shared_ptr
#include <iostream>

int main(){
	// A shared pointer is 'smart' in the sense that
	// it will automatically destruct itself.
	// We explicitly make a shared_ptr that stores a pointer to
	// an int (our object) with a value of 10.
	std::shared_ptr<int> mySharedPointer = std::make_shared<int>(10);

	std::cout << "mySharedPointer value(i.e dereferenced):" << *mySharedPointer << "\n";
	std::cout << "mySharedPointer address(i.e memory location):" << mySharedPointer << "\n";

	return 0;
}
