// Simple Caeser Shift decrypt
// 
// Load the data from data2.txt. Increment all of the values by 1 then output
// the result
// 
// You will compile this source using the following:
// 
// clang -std=c11 -mavx2 ex2.c -o ex2prog
//
// Run as normal with: 
// 
// ./exx2prog
//
#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>


// print
void print__m256(__m256 data){
	float*f = (float*)&data;
	printf("%f %f %f %f %f %f %f %f\n", f[0],f[1],f[2],f[3],f[4],f[5],f[6],f[7]);
}

int main(){

	FILE *fp;
	fp = fopen("data2.txt","r");

	int number;
	int cipher[32];
	int count = 0;

	int* c = (int*)malloc(64*sizeof(int));
	
	while( fscanf(fp,"%d", &number) > 0 ){
		printf("%d\n",number);
		cipher[count] = number;
		c[count] = number;
		count++;
	}
	
	__m256i firsteightcharacters = _mm256_loadu_si256((__m256i*)c);
	

	int* ch = (int*)&firsteightcharacters;

	printf("%c %c %c %c %c %c %c %c\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6], ch[7]);

	

/*
	printf(	"asciimessage: %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c %c \n",
		c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],
		c[8],c[9],c[10],c[11],c[12],c[13],c[14],c[15],
		c[16],c[17],c[18],c[19],c[20],c[21],c[22],c[23],
		c[24],c[25],c[26],c[27],c[28],c[29],c[30],c[31]
		);
*/
	free(c);

	fclose(fp);
	
	return 0;
}
