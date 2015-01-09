
// return value of startwith(): 
// if true, return the length of startstring
// if false, return -1

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

#endif