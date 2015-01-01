// parse_test.c
#include"parse.h"
#include<stdio.h>

int main()
{
	char* msg="<login> <account>Ruby<\\>  \r <signup>\n \r <password>crispy<\\> <nam";
	char dst[2][32];
	char* dest[2]={dst[0],dst[1]};
	char* pos=msg;

	int i;

	// parse commands consecutively
	for(i=0;i<6;i++){
		if(parse(&pos,dest))
			printf("%s: %s\n",dst[0],dest[1]);
		else
			printf("!!! Error: fragment! !!!\n");
	}

	return 0;
}