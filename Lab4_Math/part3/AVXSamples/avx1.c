// You will compile this source using the following:
// 
// clang -std=c11 -mavx2 avx1.c -o avx1prog
//
// Run as normal with: 
// 
// ./avx1prog
//
#include <stdio.h>

// In 1999 Intel introduced its set of SSE instructions (AMD released the equivalent 3DNow)
// that allowed us to perform a single instruction on multiple pieces of data. 
//
// Remember from our assembly programming instructions, we could only
// move one src to one destination at at time. This effectively meant
// working with only one register at a time.

// In this code we are going to focus on initialization intrinsics
// That is, how can we load and store data quickly more than one piece of data at a time.

// In order to get started with intrinsics, we need to alert our compiler about the commands.
// These are stored in an intriniscs header file.
#include <immintrin.h>



int main(){

	// (1) Let us say we have the following code below
	float evens[8] = {2,4,6,8,10,12,14,16};
	
	// If we want, we can process these evens all at once.
	// We do so by creating a 'vector' of data. This is not
	// to be confused with the data structure vector for those with C++ experience,
	// though the concept is similar, in that we are storing chunks of data.
	
	
	// Let's create our first AVX2 vector.
	__m256 evensAVX = _mm256_set_ps(2.0,4.0,6.0,8.0,10.0,12.0,14.0,16.0);

	// Now let's print out the vector
	float*f = (float*)&evens;
	printf("%f %f %f %f %f %f %f %f\n", f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7]);
	

	// Okay, there were a few interseting things going on with this sample.
	// The first is, what is a __m256 data type?
	//
	// The __m is the naming convention for our vector types. The 256 that follows
	// represents how many 'bits' can be stored in that vector.
	//
	// Just like our assembly commands, we have to be specific about the type of
	// data that we are storing.
	//
	// __m256 stores 256 bits of floating point data
	// __m256d stores 256 bits of double data
	// __m256i storse 256 bits of integer data
	//
	// (2) Question: If a float is 32 bits, how many floats can I store in a 256 bit
	// vector?
	//
	// (3) Question: If a double is 64 bits, how many doubles can I store in a 256 bit vector?
	//
	//
	
	// More on Data types
	//
	// Now there also exists other data types in C, such as shorts or char. Can I store 
	// those as well?
	//
	// The answer is yes, and they get stored in the 'integer' data type.
	//
	// Let us go ahead and try
	
	// Note the new type __m256i	 
	__m256i asciiMessage  = _mm256_set_epi8('a','b','c','d','e','f','g','h',
						'i','j','k','l','m','n','o','p',
						'q','r','s','t','u','v','w','x',
						'y','z','0','1','2','3','4','5');

	// Now let's print out the vector
	char* c = (char*)&asciiMessage;

	printf(	"asciimessage: %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c \n",
		c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],
		c[8],c[9],c[10],c[11],c[12],c[13],c[14],c[15],
		c[16],c[17],c[18],c[19],c[20],c[21],c[22],c[23],
		c[24],c[25],c[26],c[27],c[28],c[29],c[30],c[31]
		);
		

	// Now we have actually seen quite of new information here.
	// First off, what is the _mm256_set_epi8 command?
	//
	// Well, if we want to find out, we can navigate to this handy intrinsics guide from Intel.
	//
	// (Google 'Intel Intrinsics guide' or ty the link below
	// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=AVX,AVX2&text=_mm256_set_epi8&expand=4617
	//
	// epi stands for packed bit integers, and the 8 signifies that we 
	// are storing 8-bit integers, which are equivalent to a char.
	//
	// Take a moment to search '_mm256_set_epi' and see the other integer types
	//
	// Second, an observation is that our alphabet is backwards!
	// Why is that?
	// 
	// Well, take a look at the Intrinsics guide and see which argument number (e31,e32, etc)
	// lines up to our array. You can fix this problem by reversing the output
	// or the input into our array. You must however make sure you are consistent!
	//
	//
	// One final note with this example is the output. 
	// We have lines like the following:
	//
	// char* c = (char*)&asciiMessage;
	//
	// Remember that a pointer is just a pointer to an address.
	// If we point it to our intrinsic data type, it points to the start of that vector.
	// We then can use an offset (e.g. asciiMessage[1] to get the 2nd item) to access
	// different parts of our vector.

	return 0;

}
