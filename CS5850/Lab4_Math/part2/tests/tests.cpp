// Add your unit tests somewhere in this file
// Provide sufficient 'coverage' of funtions you have implemented.
#include "TinyMath.hpp"
#include <iostream>

// Add a 1 vector with a 0 vector.
bool unitTest1(){
  Vector3D a(1,1,1);
  Vector3D b(0,0,0);
  Vector3D c = a + b;
 
  if(c.x == 1 && c.y == 1 && c.z ==1){
    return true;
  }
    return false;
}

// TODO
bool unitTest2(){
    Vector3D a(1, 1, 1);
    a *= 3;
    
    if(a.x == 3 && a.y == 3 && a.z == 3)
        return true;
    
    return false;
}
// TODO
bool unitTest3(){
    Vector3D a(1, 1, 1);
    a /= 2;
    
    if(a.x == 0.5f && a.y == 0.5f && a.z == 0.5f)
        return true;
    
    return false;
}
// TODO
bool unitTest4(){
    Vector3D a(1, 1, 1);
    Vector3D b(2, 2, 2);
    
    a += b;
    if(a.x == 3 && a.y == 3 && a.z == 3)
        return true;
    
    return false;
}
// TODO
bool unitTest5(){
    Vector3D a(1, 1, 1);
    Vector3D b(2, 2, 2);
    
    a -= b;
    if(a.x == -1 && a.y == -1 && a.z == -1)
        return true;
    
    return false;
}
// TODO
bool unitTest6(){
    Vector3D a(3, 1, 4);
    Vector3D b(2, 2, 2);
    
    float v = Dot(a, b);

    if(v == 16)
        return true;
    
    return false;
}
// TODO
bool unitTest7(){
    Vector3D a(1, 1, 1);
    Vector3D b = a * 3.0f;
    
    if(b.x == 3 && b.y == 3 && b.z == 3)
        return true;
    return false;
}
// TODO
bool unitTest8(){
    Vector3D a(1, 1, 1);
    Vector3D b = a / 2.0f;
    
    if(b.x == 0.5f && b.y == 0.5f && b.z == 0.5f)
        return true;
    return false;
}
// TODO
bool unitTest9(){
    Vector3D a(1, 1, 1);
    Vector3D b = -a;
    
    if(b.x == -1 && b.y == -1 && b.z == -1)
        return true;
    return false;
}
// TODO
bool unitTest10(){
    Vector3D a(3, 4, 0);
    if(Magnitude(a) == 5)
        return true;
    return false;
}
// TODO
bool unitTest11(){
    Vector3D a(1,1,1);
    Vector3D b(0,0,0);
    Vector3D c = b - a;
    
    if(c.x == -1 && c.y == -1 && c.z == -1){
      return true;
    }
      return false;
    return false;
}
// TODO
bool unitTest12(){
    Vector3D a(3, 4, 0);
    Vector3D b(10, 0, 0);
    
    Vector3D c = Project(a, b);
    
    if(c.x == 3 && c.y == 0 && c.z == 0)
        return true;
    return false;
}

bool unitTest13(){
    Vector3D a(12, 16, 0);
    
    Vector3D c = Normalize(a);
    
    if(c.x == 0.6f && c.y == 0.8f && c.z == 0)
        return true;
    return false;
}

bool unitTest14(){
    Vector3D a(1, 0, 0);
    Vector3D b(0, 1, 0);
    
    Vector3D c = CrossProduct(a, b);
    
    if(c.x == 0 && c.y == 0 && c.z == 1)
        return true;
    return false;
}

bool unitTest15(){
    Matrix3D m1(1, 1, 1, 1, 2, 1, 1, 3, 1);
    Vector3D a(1, 1, 1);
    Vector3D b(2, 1, 1);
    Vector3D c(3, 1, 1);
    Matrix3D m2(a, b, c);
    
    Matrix3D m3 = m1 * m2;
    if(m3(0, 0) == 6  && m3(0, 1) == 3 && m3(0, 2) == 3 && 
       m3(1, 0) == 8  && m3(1, 1) == 4 && m3(1, 2) == 4 && 
       m3(2, 0) == 10 && m3(2, 1) == 5 && m3(2, 2) == 5)
        return true;
    return false;
}

bool unitTest16(){
    Matrix3D m1(1, 1, 1, 1, 2, 1, 1, 3, 1);
    Vector3D a(1, 3, 5);
    
    Vector3D res = m1 * a;
    
    if(res.x == 9 && res.y == 12 && res.z == 15)
        return true;
    return false;
}


// TODO: Add more tests here at your discretion
bool (*unitTests[])(void)={
    unitTest1,
    unitTest2,
    unitTest3,
    unitTest4,
    unitTest5,
    unitTest6,
    unitTest7,
    unitTest8,
    unitTest9,
    unitTest10,
    unitTest11,
    unitTest12,
    unitTest13,
    unitTest14,
    unitTest15,
    unitTest16,
    NULL
};


// ====================================================
// ================== Program Entry ===================
// ====================================================
int main(){
    unsigned int testsPassed = 0;
    // List of Unit Tests to test your data structure
    int counter =0;
    while(unitTests[counter]!=NULL){
	std::cout << "========unitTest " << counter << "========\n";
        if(true==unitTests[counter]()){
		std::cout << "passed test\n";
		testsPassed++;	
	}else{
		std::cout << "failed test, missing functionality, or incorrect test\n";
	}
        counter++;
    }

    std::cout << "\n\n" << testsPassed << " of " << counter << " tests passed\n";

    return 0;
}
