// You will compile this source using the following:
// 
// clang -std=c11 -mavx2 avx3.c -o avx3prog
//
// Run as normal with: 
// 
// ./avx3prog
//
#include <stdio.h>
#include <immintrin.h>

// So we have learned how to load data, and we can even
// print out data as we need.
//
// Let us now perform some arithmetic operations, which are the
// essential part of using the intrinsics instructions.

// print
void print__m256(__m256 data){
	float*f = (float*)&data;
	printf("%f %f %f %f %f %f %f %f\n", f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7]);
}


int main(){

	// The arithmetic instructions are name as you might assume.
	// _mm256_add_ps(__m256 a, __m256 b)
	//
	// Now let us actually investigate the intrinsics guide here
	// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=MMX,SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2,AVX,AVX2&expand=4617,130&text=_mm256_add
	//
	// What I want to draw to your attention is the 'Operation' section.
	// This is pseudo-code that shows you what is going on in the algorithm.
	// Intuitively, from 'i' which is the index of each of the
	// elements (in our case, 8, 32 bit floats) is used as an offset.
	// The destination is then set to the sum of a and b at each index.
	
	__m256 a = _mm256_set1_ps(1);
	__m256 b = _mm256_set1_ps(2);
	__m256 dst = _mm256_add_ps(a,b);

	print__m256(dst);

	// Okay, go ahead and try some other arithmetic operations.
	// Here are a few to try:
	//
	// _mm256_sub_ps
	//
	// How about working with integer data
	// What about _mm256_add_epi16 vs _mm256_adds_epi16
	//
	// Take a look at the documentation for _mm256_adds_epi16. What does saturation
	// mean?
	//
	//
	// Here is another fun one to experiment with: horizontal add
	//
	// _mm256_hadd_ps
	//
	// best shown with an image: https://www.codeproject.com/KB/cpp/874396/Fig1.jpg


	// Okay, so we have seen some interesting arithmetic add and subtraction operations,
	// but what about the classics: multiply and divide?
	//
	// _mm256_mul_ps and _mm256_div_ps
	//
	// Try multiplying two numbers and see the result.
	
	__m256 float_a = _mm256_set_ps(1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0);
	__m256 float_b = _mm256_set_ps(10.0,10.0,10.0,10.0,10.0,10.0,10.0,10.0);
	__m256 float_dst = _mm256_mul_ps(float_a,float_b);

	print__m256(float_dst);	
	

	return 0;
}
