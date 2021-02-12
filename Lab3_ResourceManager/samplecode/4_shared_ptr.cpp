// g++ -std=c++14 4_shared_ptr.cpp -o 4_shared_ptr
// 
// An alternative of the previous program.
// 
#include <memory> // Need to include this library for shared_ptr
#include <iostream>

int main(){
	// A shared pointer is 'smart' in the sense that
	// it will automatically destruct itself.

	// Shared pointers will also keep track of how many 'references'
	// they have pointing to them.

	// We explicitly make a shared_ptr that stores a pointer to
	// an int (our object) with a value of 10.
	std::shared_ptr<int> mySharedPointer = std::make_shared<int>(10);

	// As mentioned, a shared pointer can have multiple references and
	// it will keep track of how many references exist.
	std::shared_ptr<int> anotherPointer(mySharedPointer);

	std::cout << "mySharedPointer's value: " << *mySharedPointer << "\n";
	std::cout << "anotherPointer's value: " << *anotherPointer << "\n";
	
	// Great, both should be pointing to the same thing!
	// What is neat, is we can actually see how many references we haev to 
	// the shared pointer.
	// That is, it is not safe to reclaim memory automatically until all
	// of the references are gone.
	 
	std::cout << "mySharedPointer's references: " << mySharedPointer.use_count() << "\n";
	std::cout << "anotherPointer's references: " << anotherPointer.use_count() << "\n";


	return 0;
}
