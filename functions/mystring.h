
// return value of startwith(): 
// if true, return the length of startstring
// if false, return 0

#ifndef MYSTRING
#define MYSTRING

int startwith(char* string, char* startstring){
	char* ptr1=string;
	char* ptr2=startstring;
	while(*ptr2){
		if(*ptr1!=*ptr2)
			return 0;
		ptr1++;
		ptr2++;
	}
	return ptr2-startstring;
}

// Input a character array of a number, output the integer
// Side effect: *str will point to the character on the right of delimeter character
// --- Sample ---
// Input: *str="123,5", delimiter=","
// Return value: 123
// Side effect: str will point to '5'
int myatoi(char** str, char delimiter){
	int ans=0;
	char* ptr=*str;
	while(*ptr!=delimiter){
		ans = ans*10 + (*ptr-'0');
		ptr++;
	}
	ptr++;
	*str=ptr;
	return ans;
}

// read from *src, until character "delimiter", and copy all characters that were read
// into dest (delimiter excluded)
// Side effect: *src will point to the character following the delimiter
void readuntil(char** src, char* dest, char delimiter){
	char* readpos=*src;
	char* dest_ptr=dest;
	while(*readpos!=0 && *readpos!=delimiter){
		*dest_ptr=*readpos;
		readpos++;
		dest_ptr++;
	}
	*dest_ptr=0;
	readpos++;
	*src=readpos;
}		

#endif