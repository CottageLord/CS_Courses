// g++ -std=c++14 1_raw_pointer.cpp -o 1_raw_pointer
// 
// 
// 
#include <iostream>

int main(){
	
	// A regular pointer we create in the following way
	int* intPtr = new int;

	// We have to delete it when it goes out of scope
	delete intPtr;

	// Similiarly if we create an array, we need to create
	// and delete it with []'s.
	int* pointerArray = new int[50];

	// delete[] and do not forget the brackets, otherwise 
	// you only delete the first entry!
	delete[] pointerArray;


	return 0;
}
