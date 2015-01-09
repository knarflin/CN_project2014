// filter.h
// Detect dirty words in a string, and replace each of them with a word of the same length
// The length of the string should not exceed filter_buffer_size


#ifndef FILTER
#define FILTER

#include<string.h>
#include"mystring.h"
#define filter_buffer_size 8192


// When modifying the list, three code segments needs to be modified:

#define dirtycnt 3 // modifiable 1/3, number of dirty words

// constraint: the length of each pair of words must be the same
char dirty[dirtycnt][2][20]={
	{"fuck","f*ck"},
	{"shit","sh*t"},
	{"abcde","*****"}
}; // modifiable 2/3
const int dirtylen[dirtycnt]={4,4,5}; // modifiable 3/3, the length of each dirty word



char filter_buffer[filter_buffer_size];

void filter(char* str){
	int len;
	int i,j;

	len=strlen(str);
	strcpy(filter_buffer,str);
	for(i=0;i<len;i++)
		filter_buffer[i]|=0x20; // turn the letter into lower case
								// if it is not a letter, then it will be turned into
								// something strange, but this does not affect the 
								// correctness of the result
	for(i=0;i<dirtycnt;i++){
		for(j=0;j<=len-dirtylen[i];j++){
			if(startwith(&filter_buffer[j],dirty[i][0])){
				memcpy(&str[j],dirty[i][1],dirtylen[i]*sizeof(char));
			}
		}
	}
}

#endif