// You will compile this source using the following:
// 
// clang -std=c11 -mavx2 avx5.c -o avx5prog
//
// Run as normal with: 
// 
// ./avx5prog
//
#include <stdio.h>
#include <immintrin.h>

// Last we looked at masking data
// We can also rearrange data that we have loaded.
//
// This is done using permute instructions


// print
void print__m256(__m256 data){
	float*f = (float*)&data;
	printf("%f %f %f %f %f %f %f %f\n", f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7]);
}

int main(){

	// Let's start with some data here
	__m256 a = _mm256_setr_ps(1.0,2.0,3.0,4.0,1.0,2.0,3.0,4.0);	
	print__m256(a);

	// If I apply a mask, and that mask tells me where to arrange items
	// in a vector that can be incredibly useful.
	// The mask that I am going to input is in binary.
	// That is, when I insert the mask, 0b10110100
	// I should read the binary numbers in sets of two.
	// |10|11|01|00
	//  2  3  1  0
	//
	// Or is it read in the other way? Try!
	__m256 result = _mm256_permute_ps(a,0b10110100); 

	print__m256(result);

	// Discuss whether this can be a useful function?
	// Here is one particular domain in which it is used.
	// Color formats!
	//
	// Images are often stored in the format RGBA
	// Where R is red, G green, B is blue, and A is the alpha(transparnecy)
	//
	// Some image formats however are stored in BGRA, and we have to convert them.
	//
	// Can you think of other domains in which you may need to 'permute'?


	return 0;
}
