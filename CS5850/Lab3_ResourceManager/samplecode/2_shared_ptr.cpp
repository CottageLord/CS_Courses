// g++ -std=c++14 2_shared_ptr.cpp -o 2_shared_ptr
// 
// Resource: https://en.cppreference.com/w/cpp/memory/shared_ptr
// 
#include <memory> // Need to include this library for shared_ptr


int main(){
	// A shared pointer is 'smart' in the sense that
	// it will automatically destruct itself.
	std::shared_ptr<int> mySharedPointer(new int);

	return 0;
}
