// You will compile this source using the following:
// 
// clang -std=c11 -mavx2 avx2.c -o avx2prog
//
// Run as normal with: 
// 
// ./avx2prog
//
#include <stdio.h>
#include <immintrin.h>

// Now I want to talk about a few handy functions in the AVX library that
// are things we frequently do as programmers. That is, initialize our data.
// to various values.
//
// But before we do that, let us make two assumptions
// (1) 	We are going to work with floats
// (2) 	It would be nice to be able to have a function to print out
// 	floats given an __m256 data type.


// TODO: Implement this function
void print__m256(__m256 data){
	float*f = (float*)&data;
	printf("%f %f %f %f %f %f %f %f\n", f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7]);
}


int main(){

	__m256 evensAVX = _mm256_set_ps(2.0,4.0,6.0,8.0,10.0,12.0,14.0,16.0);

	// Implement your print function above.
	printf("=========Printing evens=========\n\n");
	print__m256(evensAVX);


	// Okay, now lets to experiment with a few commands to setup our data.
	//
	// TODO: Experiment with following commands below, and print out the values.
	//	 If you do not know what the command does, look it up in the intrinsics guide.	
	// 
	// _mm256_setzero_ps
	// _mm256_set1_ps
	//
	// Your output should include a vector of all zeroes and a vector all with 42.00000.
	__m256 test1 = _mm256_setzero_ps();
	__m256 test2 = _mm256_set1_ps(42.0f);

	printf("=========Printing 0's and 42's=========\n\n");
	print__m256(test1);
	print__m256(test2);


	// Those commands are nice ways to very quickly fill up a value.
	// In C we have a command 'memset' which zeros out a number of bytes in an array.
	// If we want to explicitly set the values, we have the following:
	//
	// _mm256_set_ps(0..7)
	// _mm256_setr_ps(7..0)
	//
	// Try to use each of these commands and print out the result.

	test1 = _mm256_set_ps(1,2,3,4,5,6,7,8);
	
	printf("=========Printing _mm256_set_ps=========\n\n");
	print__m256(test1);

	// Now why might we have a version of set and setr?
	// Well, again remember from our previous discussion that the order matters.
	// Intel machines are typically little-endian based architectures.
	//
	// Little-Endian means the least signficant value in the sequence is stored first.


	// Okay, so we have spent some time setting data up. But often we
	// work with data in which we load from memory. We have the ability to do so
	// using the intrinsic functions.
	//
	// One concept that becomes important, is the idea of alignment in memory.
	// That is, we need to make sure the memory address from which we load data
	// is aligned on a 32-byte boundary.
	//
	// We have previously learned about malloc which does indeed load memory that
	// is aligned, but always in multiples of 8 or 16. Let us make sure it is always in 
	// 32 byt using aligned_malloc.
	
	float* aligned_32 = (float*)aligned_alloc(32, 64 * sizeof(float));

	// Let's break that functiond down.
	// First, we cast to a float*, just as we would have done with a regular malloc.
	// The first parameter however, describes our alignment, in this case 32 bytes.
	// Remember, our 256-bit vector is divided into nice 32 byte chunks.
	//
	// |32|32|32|32|32|32|32|32| = 256 bits
	//  7  6  5  4  3  2  1  0
	//
	//
	// And why do we multiply 64 * sizeof(float)?	
	// Well, we can always check


	printf("=========Loading aligned data=========\n");
	printf("sizeof(float)=%lu\n",sizeof(float));
	
	// And hopefully whatever that number is * 64 is 256!
	// You can read more here: 
	// https://www.gnu.org/software/libc/manual/html_node/Aligned-Memory-Blocks.html
	//
	// Okay, now let us populate our data.
	//
	
	aligned_32[0] = 0.0f;
	aligned_32[1] = 1.0f;
	aligned_32[2] = 2.0f;
	aligned_32[3] = 3.0f;
	aligned_32[4] = 4.0f;
	aligned_32[5] = 5.0f;
	aligned_32[6] = 6.0f;
	aligned_32[7] = 7.0f;

	__m256 aligned_avx = _mm256_load_ps(aligned_32);
	print__m256(aligned_avx);

	// What happens if you need to load data that is unaligned?
	//
	// Investigate _mm256_loadu_ps(...) in the intrinsics guide.


	return 0;
}
